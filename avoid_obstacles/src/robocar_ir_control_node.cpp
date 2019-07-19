#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float32MultiArray.h"

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
  _RcControl.SetSteerAngle(msg_angle->data);
}
void callback_speed(const std_msgs::Float32::ConstPtr& msg_speed)
{
  ROS_INFO("Speed: [%f]", msg_speed->data);
  _RcControl.SetDriveSpeed(msg_speed->data);
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
  ros::Publisher chatter_pub = n.advertise<std_msgs::Float32MultiArray>("irsensor", 10);
  ros::Rate loop_rate(10);
  ros::Subscriber robo_angle_sub = n.subscribe("robo_angle", 10, callback_angle);
  ros::Subscriber robo_speed_sub = n.subscribe("robo_speed", 10, callback_speed);
  ros::spin();
  
  OBSTACLE_VALUE obstacle;
  
  while(ros::ok())
  {   
      
    std_msgs::Float32MultiArray msg;
    msg.data.resize(6);
    //std::stringstream ss;
    //ss << "showtime" ;
    //msg.data = ss.str();
    memset(&obstacle, 0, sizeof(OBSTACLE_VALUE));
    _RcControl.GetObstacleSensorInfoReq(&obstacle);
    msg.data[0] = obstacle.obstacle[1];
    msg.data[1] = obstacle.obstacle[2];
    msg.data[2] = obstacle.obstacle[3];
    msg.data[3] = obstacle.obstacle[4];
    msg.data[4] = obstacle.obstacle[5];
    msg.data[5] = obstacle.obstacle[6];


    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();

  }

  return 0;
}
