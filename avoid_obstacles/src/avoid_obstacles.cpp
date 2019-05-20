#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include <sstream>
#include "avoid_obstacles/BoundingBoxes.h"
//#include "darknet_ros/YoloObjectDetector.hpp"
using namespace std;

class Listener 
{
  public:
    int num;
    string Class_name;
    float prob;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    int real_y_max;

    void chatterCallback(const avoid_obstacles::BoundingBoxes::ConstPtr& msg);
};

void Listener::chatterCallback(const avoid_obstacles::BoundingBoxes::ConstPtr& msg)
{
  num = msg->bounding_boxes.size();
  real_y_max = 0; 
  //prob = msg->bounding_boxes[0].probability;
  //ROS_INFO("I heard bounding_box: [%d]", num);
  for(int i = 0; i< num ;i++){
  int number=i;
  if(msg->bounding_boxes[i].ymax<400){
    Class_name = msg->bounding_boxes[i].class_name.c_str();
    prob = msg->bounding_boxes[i].probability;
    x_min = msg->bounding_boxes[i].xmin;
    x_max = msg->bounding_boxes[i].xmax;
    y_min = msg->bounding_boxes[i].ymin;
    y_max = msg->bounding_boxes[i].ymax;
  }
  if (real_y_max < y_max && prob > 0.4 )//&& Class_name!="chair")
  {
    real_y_max = y_max;
  }
  
  /*
  ROS_INFO("I heard class_name: %d [%s]", number,msg->bounding_boxes[i].class_name.c_str());
  ROS_INFO("I heard probability: %d [%f]", number,msg->bounding_boxes[i].probability);
  ROS_INFO("I heard xmin: %d [%lu]", number,msg->bounding_boxes[i].xmin);
  ROS_INFO("I heard xmax: %d [%lu]", number,msg->bounding_boxes[i].xmax);
  ROS_INFO("I heard ymin: %d [%lu]", number,msg->bounding_boxes[i].ymin);
  ROS_INFO("I heard ymax: %d [%lu]", number,msg->bounding_boxes[i].ymax);
  //ROS_INFO("I heard bounding_box: [%d]", num);*/
  }
}
int main(int argc, char **argv)
{
  
  //ros::init(argc, argv, "listener");  
  ros::init(argc, argv, "obstacle_avoidance");   
  ros::NodeHandle n;   
  Listener box;
  //ros::Subscriber sub = n.subscribe("chatter", 1000, chatterCallback);
  ros::Subscriber bb_sub = n.subscribe("/darknet_ros/bounding_boxes", 1000, &Listener::chatterCallback, &box);
  
  //ROS_INFO( "I heard xmin: %d ",listener.bb_sub.probability);
  
  ros::Publisher robo_angle_pub = n.advertise<std_msgs::Float32>("robo_angle", 1000);
  ros::Publisher robo_speed_pub = n.advertise<std_msgs::Float32>("robo_speed", 1000);

  ros::Rate loop_rate(10);   
  
  //ROS_INFO( bb_sub);
  int last_speed = 0;
  int count = 0;
  while (ros::ok())
  {
    /*//ROS_INFO( "I heard probability: %f ",box.prob);
    ROS_INFO("I heard class_name: %s", box.Class_name.c_str());
    ROS_INFO("I heard probability: %f", box.prob);
    ROS_INFO("I heard xmin: %i", box.x_min);
    ROS_INFO("I heard xmax: %i", box.x_max);
    ROS_INFO("I heard ymin: %i", box.y_min);
    ROS_INFO("I heard ymax: %i", box.y_max);*/
    if(box.Class_name.c_str()!="chair"){
      ROS_INFO("I heard class_name: %s", box.Class_name.c_str());
      ROS_INFO("I heard probability: %f", box.prob);
      ROS_INFO("I heard xmin: %i", box.x_min);
      ROS_INFO("I heard xmax: %i", box.x_max);
      ROS_INFO("I heard ymin: %i", box.y_min);
      ROS_INFO("I heard ymax: %i", box.y_max);
      ROS_INFO("I heard real ymax: %i", box.real_y_max);
    }
    std_msgs::Float32 msg_angle;
    std_msgs::Float32 msg_speed;
    int y_thresh = 200;
    if (box.real_y_max<y_thresh  )// && last_speed != 0)
    {
      msg_angle.data = 0;
      msg_speed.data = 30;
      last_speed = msg_speed.data;
    }
    else if(box.real_y_max>y_thresh && (box.prob> 0.5||box.Class_name.c_str()=="person"))
    {
      msg_angle.data = 0;
      msg_speed.data = 0;
      last_speed = msg_speed.data;
    }
    //msg_angle.data = 10;
    //msg_speed.data = 10;

    //ROS_INFO("%s", msg_angle.data.c_str());


    robo_angle_pub.publish(msg_angle);
    robo_speed_pub.publish(msg_speed);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }

/**/
  ros::spin();

  return 0;
}
