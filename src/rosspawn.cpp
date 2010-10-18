
/***************************************************************************
 *  rosspawn.cpp - ROSspawn main application
 *
 *  Created: Tue Aug  3 17:06:44 2010
 *  Copyright  2010  Tim Niemueller [www.niemueller.de]
 *             2010  Carnegie Mellon University
 *             2010  Intel Labs Pittsburgh, Intel Research
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE file in the base directory.
 */

#include "rosspawn/NodeAction.h"
#include "rosspawn/ListAvailable.h"
#include "rosspawn/ListLoaded.h"
#include "rosspawn/NodeEvent.h"

#include <ros/ros.h>
#include <ros/console.h>
#include <rospack/rospack.h>

#include <map>
#include <string>
#include <utility>
#include <fstream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <regex.h>

// For now we only accept binaries which are in a bin directory. This makes
// thinks much less clutered, otherwise add  "%s/%s" to the array
const char *test_paths[] = { "%s/bin/%s" };

class RosSpawnMain
{
 public:
  RosSpawnMain(ros::NodeHandle &n)
  : __n(n)
  {
    __children_wait_thread = boost::thread(boost::bind(&RosSpawnMain::wait_thread, this));
    if (regcomp(&__re_alnum, "^[[:alnum:]]+$", REG_EXTENDED) != 0) {
      throw ros::Exception("Failed to compile regex");
    }

    __use_acceptable_modules_file = n.getParam("/rosspawn/acceptable_modules_file",
					       __acceptable_modules_file);

    rospack::ROSPack rospack;

    if (__use_acceptable_modules_file) {
      ROS_INFO("Using acceptable modules file %s", __acceptable_modules_file.c_str());
      std::ifstream f(__acceptable_modules_file.c_str());
      while (! (f.fail() || f.eof()) ) {
	std::string mod;
	f >> mod;
	for (rospack::VecPkg::iterator i = rospack::Package::pkgs.begin();
	     i != rospack::Package::pkgs.end(); ++i) {
	  if ((*i)->name == mod) {
	    __search_paths.push_back((*i)->path);
	  }
	}
      }
    } else {
      std::string ros_root_bin = getenv("ROS_ROOT");
      if (ros_root_bin == "") {
	throw ros::Exception("Failed to read ROS_ROOT environment variable");
      }
      ros_root_bin += "/bin";
      __search_paths.push_back(ros_root_bin);
      for (rospack::VecPkg::iterator i = rospack::Package::pkgs.begin();
	   i != rospack::Package::pkgs.end(); ++i) {
	__search_paths.push_back((*i)->path);
      }
    }

    __srv_start = n.advertiseService("/rosspawn/start", &RosSpawnMain::start_node, this);
    __srv_stop = n.advertiseService("/rosspawn/stop", &RosSpawnMain::stop_node, this);
    __srv_pause = n.advertiseService("/rosspawn/pause", &RosSpawnMain::pause_node, this);
    __srv_continue = n.advertiseService("/rosspawn/continue", &RosSpawnMain::continue_node, this);
    __srv_list_loaded = n.advertiseService("/rosspawn/list_loaded",
					   &RosSpawnMain::list_loaded, this);
    __srv_list_avail = n.advertiseService("/rosspawn/list_available",
					  &RosSpawnMain::list_available, this);

    __pub_node_events = n.advertise<rosspawn::NodeEvent>("rosspawn/node_events", 10);
  }

  ~RosSpawnMain()
  {
    // We use native pthread calls here since Boost does not allow to cancel a thread.
    // I'm not going back to the stoneage and do a polling wait() in the thread and
    // waste power just to account for the sluggish Boost API.
    void *dont_care;
    pthread_cancel(__children_wait_thread.native_handle());
    pthread_join(__children_wait_thread.native_handle(), &dont_care);
  }

  std::string find_valid(std::string &progname)
  {
    if (regexec(&__re_alnum, progname.c_str(), 0, NULL, 0) == REG_NOMATCH) {
      throw ros::Exception("Invalid program name");
    }

    for (ChildrenMap::iterator i = __children.begin(); i != __children.end(); ++i) {
      if (i->second.first == progname) {
	throw ros::Exception("Program is already running");
      }
    }

    for (std::list<std::string>::iterator i = __search_paths.begin();
	 i != __search_paths.end(); ++i) {
      for (unsigned int j = 0; (j < sizeof(test_paths) / sizeof(const char *)); ++j) {
        char *tmp;
	if (asprintf(&tmp, test_paths[j], i->c_str(), progname.c_str()) != -1) {
	  ROS_DEBUG("Trying %s", tmp);
	  std::string path = tmp;
	  free(tmp);
	  if (access(path.c_str(), X_OK) == 0) {
	    return path;
	  }
	}
      }
    }

    throw ros::Exception("No program with the requested name found"); 
  }

  bool fork_and_exec(std::string &progname)
  {
    std::string p;
    try {
      p = find_valid(progname);
    } catch (ros::Exception &e) {
      ROS_ERROR("Error starting %s: %s", progname.c_str(), e.what());
      return false;
    }

    pid_t pid = fork();
    if (pid == -1) {
      return false;
    } else if (pid == 0) {
      // child
      setsid();
      signal(SIGINT, SIG_IGN);
      ROS_INFO("Running %s from path %s", progname.c_str(), p.c_str());
      fclose(stdout);
      fclose(stdin);
      fclose(stderr);
      execl(p.c_str(), p.c_str(), NULL);
    } else {
      ROS_DEBUG("Child PID %i", pid);
      boost::mutex::scoped_lock lock(__children_mutex);
      __children[pid] = make_pair(progname, p);
      if (__children.size() == 1) {
	__children_cond.notify_all();
      }
      rosspawn::NodeEvent msg;
      msg.header.stamp.setNow(ros::Time::now());
      msg.event_type = rosspawn::NodeEvent::NODE_STARTED;
      msg.node_name = progname;
      __pub_node_events.publish(msg);
    }

    return true;
  }

  void wait_thread()
  {
    while (ros::ok()) {
      boost::unique_lock<boost::mutex> lock(__children_mutex);
      while (__children.empty()) {
	__children_cond.wait(lock);
      }

      int status = 0;
      lock.unlock();
      pid_t pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
      if (pid == -1)  continue;
      lock.lock();

      rosspawn::NodeEvent msg;
      msg.event_type = rosspawn::NodeEvent::NODE_DIED;
      msg.node_name = __children[pid].first;

      // Debug output
      if (WIFEXITED(status)) {
	ROS_INFO("%i/%s exited, status=%d", pid,
		 __children[pid].first.c_str(), WEXITSTATUS(status));
	char *tmp;
	if (asprintf(&tmp, "%s (PID %i) exited, status=%d",
		     __children[pid].first.c_str(), pid, WEXITSTATUS(status)) != -1) {
	  msg.message = tmp;
	  free(tmp);
	}
      } else if (WIFSIGNALED(status)) {
	ROS_INFO("%i/%s killed by signal %d", pid,
		  __children[pid].first.c_str(), WTERMSIG(status));
	char *tmp;
	if (asprintf(&tmp, "%s (PID %i) killed by signal %d",
		     __children[pid].first.c_str(), pid, WTERMSIG(status)) != -1) {
	  msg.message = tmp;
	  free(tmp);
	}
      } else if (WIFSTOPPED(status)) {
	ROS_INFO("%i/%s stopped by signal %d", pid,
		  __children[pid].first.c_str(), WSTOPSIG(status));
	char *tmp;
	msg.event_type = rosspawn::NodeEvent::NODE_PAUSED;
	if (asprintf(&tmp, "%s (PID %i) stopped by signal %d",
		     __children[pid].first.c_str(), pid, WSTOPSIG(status)) != -1) {
	  msg.message = tmp;
	  free(tmp);
	}
      } else if (WIFCONTINUED(status)) {
	ROS_INFO("%i/%s continued", pid, __children[pid].first.c_str());
	char *tmp;
	msg.event_type = rosspawn::NodeEvent::NODE_CONTINUED;
	if (asprintf(&tmp, "%s (PID %i) continued",
		     __children[pid].first.c_str(), pid) != -1) {
	  msg.message = tmp;
	  free(tmp);
	}
      }

      if (WIFEXITED(status) || WIFSIGNALED(status)) {
	if (WIFSIGNALED(status)) {
	  int sig = WTERMSIG(status);
	  if (sig == SIGSEGV) {
	    // inform about faulty program
	    ROS_WARN("Program %s (%s) died with segfault", __children[pid].first.c_str(),
		     __children[pid].second.c_str());

	    char *tmp;
	    msg.event_type |= rosspawn::NodeEvent::NODE_SEGFAULT;
	    if (asprintf(&tmp, "%s (PID %i) died with segfault",
			 __children[pid].first.c_str(), pid) != -1) {
	      msg.message = tmp;
	      free(tmp);
	    }
	  }
	}
	__children.erase(pid);
      }

      __pub_node_events.publish(msg);
    }
  }

  std::string get_process_state(pid_t pid)
  {
    char *procpath;
    if (asprintf(&procpath, "/proc/%i/stat", pid) != -1) {
      FILE *f = fopen(procpath, "r");
      if (f) {
	int pid;
	char *program;
	char state[2]; state[1] = 0;
	if (fscanf(f, "%d %as %c", &pid, &program, state) == 3) {
	  free(program);
	  return state;
	}
	fclose(f);
      }
      free(procpath);
    }

    return "?";
  }

  pid_t get_pid(std::string &node_file_name)
  {
    for (ChildrenMap::iterator i = __children.begin(); i != __children.end(); ++i) {
      if (i->second.first == node_file_name) {
	return i->first;
      }
    }
    return 0;
  }

  bool start_node(rosspawn::NodeAction::Request &req,
		  rosspawn::NodeAction::Response &resp)
  {
    return fork_and_exec(req.node_file_name);
  }

  bool send_signal(std::string &node_file_name, int signum)
  {
    pid_t pid = get_pid(node_file_name);
    if (pid != 0) {
      ROS_INFO("Sending signal %s (%i) to %s (PID %i)", strsignal(signum), signum,
	       __children[pid].first.c_str(), pid);
      ::kill(pid, signum);
      return true;
    } else {
      return false;
    }
  }

  bool pause_node(rosspawn::NodeAction::Request &req,
		  rosspawn::NodeAction::Response &resp)
  {
    return send_signal(req.node_file_name, SIGSTOP);
  }

  bool continue_node(rosspawn::NodeAction::Request &req,
		     rosspawn::NodeAction::Response &resp)
  {
    return send_signal(req.node_file_name, SIGCONT);
  }

  bool stop_node(rosspawn::NodeAction::Request &req,
		 rosspawn::NodeAction::Response &resp)
  {
    pid_t pid = get_pid(req.node_file_name);
    if (pid != 0) {
      std::string state = get_process_state(pid);
      ROS_INFO("Sending signal %s (%i) to %s (PID %i)", strsignal(SIGINT), SIGINT,
	       __children[pid].first.c_str(), pid);
      ::kill(pid, SIGINT);

      if (state == "T") {
	ROS_INFO("Process %s (PID %i) was stopped, sending %s (%i)",
		 __children[pid].first.c_str(), pid, strsignal(SIGCONT), SIGCONT);
	::kill(pid, SIGCONT);
      }
      return true;
    } else {
      return false;
    }
  }

  bool list_loaded(rosspawn::ListLoaded::Request &req,
		   rosspawn::ListLoaded::Response &resp)
  {
    for (ChildrenMap::iterator i = __children.begin(); i != __children.end(); ++i) {
      resp.nodes.clear();
      resp.nodes.push_back(i->second.first);
    }
    return true;
  }

  bool list_available(rosspawn::ListAvailable::Request &req,
		      rosspawn::ListAvailable::Response &resp)
  {
    resp.bin_files.clear();
    for (std::list<std::string>::iterator i = __search_paths.begin();
	 i != __search_paths.end(); ++i) {
      for (unsigned int j = 0; (j < sizeof(test_paths) / sizeof(const char *)); ++j) {
        char *tmp;
	if (asprintf(&tmp, test_paths[j], i->c_str(), "") != -1) {
	  struct stat s;
	  if (stat(tmp, &s) == 0) {
	    if (S_ISDIR(s.st_mode) && (access(tmp, X_OK) == 0)) {
	      // check for files
	      DIR *d = opendir(tmp);
	      if (d != NULL) {
		struct dirent de, *deres;
		if ((readdir_r(d, &de, &deres) == 0) && (deres != NULL)) {
		  do {
		    char *tmp2;
		    if (asprintf(&tmp2, test_paths[j], i->c_str(), de.d_name) != -1) {
		      struct stat filestat;
		      if (stat(tmp2, &filestat) == 0) {
			if (S_ISREG(filestat.st_mode) && (access(tmp2, X_OK) == 0)) {
			  resp.bin_files.push_back(de.d_name);
 			}
		      }
		      free(tmp2);
		    }
		  } while ((readdir_r(d, &de, &deres) == 0) && (deres != NULL));
		}
		closedir(d);
	      }
	    }
	  }
	  free(tmp);
	}
      }
    }
    return true;
  }



 private:
  ros::NodeHandle &__n;
  ros::Publisher     __pub_node_events;
  ros::ServiceServer __srv_start;
  ros::ServiceServer __srv_stop;
  ros::ServiceServer __srv_pause;
  ros::ServiceServer __srv_continue;
  ros::ServiceServer __srv_list_loaded;
  ros::ServiceServer __srv_list_avail;

  std::list<std::string> __search_paths;

  typedef std::map<int, std::pair<std::string, std::string> > ChildrenMap;
  ChildrenMap                __children;
  boost::mutex               __children_mutex;
  boost::condition_variable  __children_cond;
  boost::thread              __children_wait_thread;

  bool                   __use_acceptable_modules_file;
  std::string            __acceptable_modules_file;
  std::list<std::string> __acceptable_modules;
  
  regex_t __re_alnum;
};

int
main(int argc, char **argv)
{
  ros::init(argc, argv, "rosspawn");
  ros::NodeHandle n;
 
  RosSpawnMain rosspawn(n);

  ROS_INFO("Ready to roll");
  ros::spin();
}
