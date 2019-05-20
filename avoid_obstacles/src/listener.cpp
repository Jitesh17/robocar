#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int32.h"
#include <sstream>
#include "avoid_obstacles/BoundingBoxes.h"
//#include "darknet_ros/YoloObjectDetector.hpp"

void chatterCallback(const avoid_obstacles::BoundingBoxes::ConstPtr& msg)
{ 
  //int a=4;
  ROS_INFO("I heard bounding_box:");
  ROS_INFO("I heard bounding_box: [%f]", msg->probability);
  //ROS_INFO(msg->bounding_boxes->xmin);
  //ROS_INFO("[%i]",a);
  //int num=msg.Class.size();
  //ROS_INFO("I susclibed [%i]", num);
  //ROS_INFO(msg.bounding_boxes);

}
/*
void chatterCallback(const avoid_obstacles::BoundingBox::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%d]", msg->xmin);
  ROS_INFO("I heard:");
}*//*
void chatterCallback(const avoid_obstacles::BoundingBox& msg)
{
  //ROS_INFO("I heard: [%d]", msg->xmin);
  ROS_INFO("I heard:");
}
*/
int main(int argc, char **argv)
{
  //ros::init(argc, argv, "listener");  
  ros::init(argc, argv, "obstacle_avoidance");   
  ros::NodeHandle n;   
  //ros::Subscriber sub = n.subscribe("sensor", 10, chatterCallback);
  ros::Subscriber bb_sub = n.subscribe("/darknet_ros/bounding_boxes", 1000, chatterCallback);
  ros::Rate loop_rate(10);
  int count = 0;
  while (ros::ok())
  {
// %EndTag(ROS_OK)%
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
// %Tag(FILL_MESSAGE)%
    //std::stringstream ss;
    //ss << "testgood " << count;
    //msg.data = ss.str();
// %EndTag(FILL_MESSAGE)%

// %Tag(ROSCONSOLE)%
    //ROS_INFO("%s", msg.data.c_str());
// %EndTag(ROSCONSOLE)%

// %Tag(SPINONCE)%
    ros::spinOnce();
// %EndTag(SPINONCE)%

// %Tag(RATE_SLEEP)%
    loop_rate.sleep();
// %EndTag(RATE_SLEEP)%
    ++count;
  }

  return 0;
}



