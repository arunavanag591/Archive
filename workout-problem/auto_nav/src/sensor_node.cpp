#include <auto_nav/sensor_description.h>

int main(int argc, char **argv){

	ros::init(argc, argv, "SENSOR_NODE");
	ros::NodeHandle nh;
	SensorNetwork sensor_network;
	sensor_network.Initialize();
	ros::spin();
	return 0;

}
