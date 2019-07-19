#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include "test_zmp/RcControl.h"
#include "std_msgs/Float32MultiArray.h"
#include <memory.h>
#include <iostream>
#include <termios.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

using namespace zmp::zrc;

int main(int argc, char **argv)
{
ros::init(argc, argv, "irsensor");

ros::NodeHandle n;
ros::Publisher chatter_pub = n.advertise<std_msgs::Float32MultiArray>("irsensor", 1000);
ros::Rate loop_rate(10);
RcControl _RcControl;
_RcControl.init();
_RcControl.Start();
OBSTACLE_VALUE obstacle;
//msg.data.resize(6);

while(ros::ok())
{   
    
    std_msgs::Float32MultiArray msg;
    msg.data.resize(6);
    //std::stringstream ss;
    //ss << "showtime" ;
    //msg.data = ss.str();
    memset(&obstacle, 0, sizeof(OBSTACLE_VALUE));
    _RcControl.GetObstacleSensorInfoReq(&obstacle);
/*		printf("0=%dmm, 1=%dmm, 2=%dmm, 3=%dmm, 4=%dmm, 5=%dmm, 6=%dmm, 7=%dmm\n",
				obstacle.obstacle[0], obstacle.obstacle[1],
				obstacle.obstacle[2], obstacle.obstacle[3],
				obstacle.obstacle[4], obstacle.obstacle[5],
				obstacle.obstacle[6], obstacle.obstacle[7]);*/
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
