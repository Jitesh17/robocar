#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"

#include <string>
#include <sstream>
#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "RcControl.h"

using namespace zmp::zrc;
/**
 * This code receives angle and speed commands and moves the robocar accordingly.
**/


float angle = 0;
int speed = 0;
int set_v = 0;
int set_t = 0;
bool sign = false;
float set_a = 0;
bool period = false;
bool loop = true;
RcControl  _RcControl;

void callback_angle(const std_msgs::Float32::ConstPtr& msg_angle)
{
  ROS_INFO("Angle: [%f]", msg_angle->data);
  _RcControl.SetSteerAngle(20);
}
void callback_speed(const std_msgs::Float32::ConstPtr& msg_speed)
{
  ROS_INFO("Speed: [%f]", msg_speed->data);
  _RcControl.SetDriveSpeed(20);
}



int main(int argc, char **argv)
{


  ros::init(argc, argv, "control_robocar");


  ros::NodeHandle n;


  _RcControl.init();
  _RcControl.Start();
 
  _RcControl.SetReportFlagReq(0x0f);
  _RcControl.SetServoEnable(1);
  _RcControl.SetMotorEnableReq(1);
  _RcControl.SetDriveSpeed(0);
  _RcControl.SetSteerAngle(0);

  ros::Subscriber robo_angle_sub = n.subscribe("robo_angle", 1000, callback_angle);
  ros::Subscriber robo_speed_sub = n.subscribe("robo_speed", 1000, callback_speed);


  ros::spin();

  return 0;
}
