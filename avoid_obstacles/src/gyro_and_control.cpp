#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int32.h"
#include "test_zmp/RcControl.h"
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

#include <sensor_msgs/Joy.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

#include <chrono>
#include <math.h>

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
DRIVE_VALUE drive;
auto t_old = std::chrono::system_clock::now();
SENSOR_VALUE sensor;
double w0 = 0;

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

void odom_calc(double &state_odom_x,double &state_odom_y,double &state_odom_th){
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */

    double x_new, y_new, theta_new ;
    /////////calculate distance/////////////////
    auto t_new =std::chrono::system_clock::now();
    //current_time2 = ros::Time::now();			
    auto msec =std::chrono::duration_cast<std::chrono::milliseconds>(t_new - t_old).count();
    t_old = t_new;
    //ros::Duration msec = current_time2 - current_time1;
    int num = 255;
    memset(&drive, 0, sizeof(DRIVE_VALUE));
    _RcControl.GetServoInfoReq(1, PRESENT_POSITION_L, PRESENT_VOLTS_H - PRESENT_POSITION_L, &drive);
    printf("temp=%d C\n",drive.present_temperature);

    memset(&sensor, 0, sizeof(SENSOR_VALUE));
    _RcControl.GetSensorInfoReq(&sensor);
   
    double w1 = -sensor.gyro*M_PI/180; //degree/sec
    double v =( sensor.enc_3 + sensor.enc_4)/2;
    double t =msec;
    
    //t1 = t1 + t;
    theta_new = state_odom_th + (w0+w1)*t/1000/2 +0.02*t/1000;// 0.004*t/1000*(1+(drive.present_temperature-40)*0.0012) ;
    w0=w1; 
    x_new = state_odom_x + v*cos(theta_new)*t/1000;
    y_new = state_odom_y + v*sin(theta_new)*t/1000;
    ///////////////////////////////////////////	
    /*---- position_output ----*/
        state_odom_x= x_new;
	state_odom_y= y_new;
	state_odom_th= theta_new;
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "gyro_and_control");
//////////   
  tf::TransformBroadcaster odom_broadcaster;
  ros::Time current_time;
  current_time = ros::Time::now();
///////////
  


  ros::NodeHandle n;


  _RcControl.init();
  _RcControl.Start();
  ros::Rate r(30.0);//30hz 
  _RcControl.SetReportFlagReq(0x0f);
  _RcControl.SetServoEnable(1);
  _RcControl.SetMotorEnableReq(1);
  _RcControl.SetDriveSpeed(0);
  _RcControl.SetSteerAngle(0);
OBSTACLE_VALUE obstacle;

  ros::Subscriber robo_angle_sub = n.subscribe("robo_angle", 10, callback_angle);
  ros::Subscriber robo_speed_sub = n.subscribe("robo_speed", 10, callback_speed);
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
  ros::Publisher chatter_pub = n.advertise<std_msgs::Float32MultiArray>("irsensor", 10);

//  ros::spin();
    double state_odom_x=0.0;//オドメトリX座標[m]
    double state_odom_y=0.0;//オドメトリY座標[m]
    double state_odom_th=0.0; //オドメトリ姿勢[rad]
   
  std_msgs::Float32MultiArray msg;

 while(n.ok()){
    ros::spinOnce();
//    ros::Subscriber sub = n.subscribe("joy", 1000, chatterCallback);
    odom_calc(state_odom_x,state_odom_y,state_odom_th);
    current_time = ros::Time::now();

    //tf map->odom
    geometry_msgs::TransformStamped map_trans;
    map_trans.header.stamp = current_time;
    map_trans.header.frame_id = "map";
    map_trans.child_frame_id = "odom";
    map_trans.transform.translation.x = state_odom_x;
    map_trans.transform.translation.y = state_odom_y;
    map_trans.transform.translation.z = 0.0;
    geometry_msgs::Quaternion map_quat = tf::createQuaternionMsgFromYaw(state_odom_th);
    map_trans.transform.rotation = map_quat;
    odom_broadcaster.sendTransform(map_trans);

    current_time = ros::Time::now();
    //tf odom->base_link
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = state_odom_x/1000.0;
    odom_trans.transform.translation.y = state_odom_y/1000.0;
    odom_trans.transform.translation.z = 0.0;
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(state_odom_th);
    odom_trans.transform.rotation = odom_quat;
    odom_broadcaster.sendTransform(odom_trans);
    odom_broadcaster.sendTransform(
      tf::StampedTransform(
        //tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.125, 0.0, 0.05)),
        tf::Transform(tf::Quaternion(1, 0, 0, 0), tf::Vector3(0.125, 0.0, 0.05)),
        ros::Time::now(),"base_link", "base_scan"));
    odom_broadcaster.sendTransform(
      tf::StampedTransform(
        tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.01, 0.0, 0.01)),
        ros::Time::now(),"map", "odom"));
    //next, we'll publish the odometry message over ROS
    nav_msgs::Odometry odom;
    odom.header.stamp = current_time;
    odom.header.frame_id = "odom";

    //set the position
    odom.pose.pose.position.x =state_odom_x/1000.0;
    odom.pose.pose.position.y =state_odom_y/1000.0;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation =odom_quat;

    //publish the message
    odom_pub.publish(odom);

 //   printf("x:%f y:%f　θ:%fdeg \n",state_odom_x,state_odom_y,state_odom_th*180/3.16);  
//printf("%ld",current_time);
//ROS_INFO("TIME:[%f]  \n",odem->header.stamp());
ROS_INFO("θ:%fdeg",state_odom_th*180/M_PI);  

    ///irsensor
  
    msg.data.resize(6);
    memset(&obstacle, 0, sizeof(OBSTACLE_VALUE));
    _RcControl.GetObstacleSensorInfoReq(&obstacle);
    msg.data[0] = obstacle.obstacle[1];
    msg.data[1] = obstacle.obstacle[2];
    msg.data[2] = obstacle.obstacle[3];
    msg.data[3] = obstacle.obstacle[4];
    msg.data[4] = obstacle.obstacle[5];
    msg.data[5] = obstacle.obstacle[6];
    //publish irsensor
    chatter_pub.publish(msg);


    r.sleep();
    }
  return 0;
}
