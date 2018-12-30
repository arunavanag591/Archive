#include <tcp_comms/tcp_comms.hh>

int main(int argc, char **argv){

	ros::init(argc, argv, "TCP_COMMS_NODE");
	ros::NodeHandle nh;
	TCPCommsPassive tcp_passive_node;
	tcp_passive_node.initialize(nh);
	ros::spin();
	return 0;

}
