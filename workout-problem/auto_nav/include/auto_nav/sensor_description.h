#pragma once
#ifndef SENSOR_DESCRIPTION
#define SENSOR_DESCRIPTION


#include <ros/ros.h>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Bool.h>

#include <string>
#include <sstream>

//using namespace std;

class SensorNetwork{
	ros::Subscriber sensor_topic_1, sensor_topic_2, sensor_topic_3, sensor_topic_4;
	ros::Publisher sensor_output;

public:
	SensorNetwork();
	~SensorNetwork();
	void Initialize();
	void sensor_1();
	void sensor_2();
	void sensor_3();
	void sensor_4();
	void sensor_5();

};

#endif
