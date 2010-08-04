
/***************************************************************************
 *  rosspawn.cpp - ROSspawn main application
 *
 *  Created: Tue Aug  3 17:06:44 2010
 *  Copyright  2010  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#include "rosspawn/StartNode.h"
#include "rosspawn/StopNode.h"
#include "rosspawn/ListLoaded.h"

#include <ros/ros.h>
#include <ros/console.h>
#include <rospack/rospack.h>

#include <map>
#include <string>
#include <utility>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <regex.h>

const char *test_paths[] = { "%s/%s", "%s/bin/%s" };

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
    std::string ros_root_bin = getenv("ROS_ROOT");
    if (ros_root_bin == "") {
      throw ros::Exception("Failed to read ROS_ROOT environment variable");
    }
    ros_root_bin += "/bin";
    __search_paths.push_back(ros_root_bin);
    rospack::ROSPack rospack;
    for (rospack::VecPkg::iterator i = rospack::Package::pkgs.begin();
	 i != rospack::Package::pkgs.end(); ++i) {
      __search_paths.push_back((*i)->path);
    }
  }

  void stop()
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
      pid_t pid = wait(&status);
      if (pid == -1)  continue;
      lock.lock();

      // Debug output
      if (WIFEXITED(status)) {
	ROS_INFO("%i/%s exited, status=%d", pid,
		  __children[pid].first.c_str(), WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
	ROS_INFO("%i/%s killed by signal %d", pid,
		  __children[pid].first.c_str(), WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
	ROS_INFO("%i/%s stopped by signal %d", pid,
		  __children[pid].first.c_str(), WSTOPSIG(status));
      } else if (WIFCONTINUED(status)) {
	ROS_INFO("%i/%s continued", pid, __children[pid].first.c_str());
      }

      if (WIFEXITED(status) || WIFSIGNALED(status)) {
	if (WIFSIGNALED(status)) {
	  int sig = WTERMSIG(status);
	  if (sig == SIGSEGV) {
	    // inform about faulty program
	    ROS_WARN("Program %s (%s) died with segfault", __children[pid].first.c_str(),
		     __children[pid].second.c_str());
	  }
	}
	__children.erase(pid);
      }
    }
  }

  bool start_node(rosspawn::StartNode::Request &req,
		  rosspawn::StartNode::Response &resp)
  {
    return fork_and_exec(req.node_file_name);
  }

  bool stop_node(rosspawn::StopNode::Request &req,
		  rosspawn::StopNode::Response &resp)
  {
    for (ChildrenMap::iterator i = __children.begin(); i != __children.end(); ++i) {
      if (i->second.first == req.node_file_name) {
	kill(i->first, SIGINT);
	return true;
      }
    }
    return false;
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



 private:
  ros::NodeHandle &__n;
  std::list<std::string> __search_paths;

  typedef std::map<int, std::pair<std::string, std::string> > ChildrenMap;
  ChildrenMap                __children;
  boost::mutex               __children_mutex;
  boost::condition_variable  __children_cond;
  boost::thread              __children_wait_thread;

  regex_t __re_alnum;
};

int
main(int argc, char **argv)
{
  ros::init(argc, argv, "rosspawn");
  ros::NodeHandle n;
 
  RosSpawnMain rosspawn(n);

  ros::ServiceServer start_srv = n.advertiseService("/rosspawn/start",
						    &RosSpawnMain::start_node, &rosspawn);

  ros::ServiceServer stop_srv = n.advertiseService("/rosspawn/stop",
						    &RosSpawnMain::stop_node, &rosspawn);

  ros::ServiceServer lstld_srv = n.advertiseService("/rosspawn/list_loaded",
						    &RosSpawnMain::list_loaded, &rosspawn);

  ROS_INFO("Ready to roll");
  ros::spin();
  rosspawn.stop();
}
