
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

#include <ros/ros.h>

class RosSpawnMain
{
 public:
  RosSpawnMain(ros::NodeHandle &n)
    : __n(n)
  {
  }

  int run()
  {
  }

 private:
  ros::NodeHandle &__n;
};

int
main(int argc, char **argv)
{
  ros::init(argc, argv, "rosspawn");
  ros::NodeHandle n;

  RosSpawnMain rosspawn(n);
  return rosspawn.run();
}
