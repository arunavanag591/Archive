#include <tcp_comms/tcp_Comms.hh>

int main(int argc, char **argv){

	ros::init(argc, argv, "tcp_COMMS_TEST_NODE");
	ros::NodeHandle nh;
	tcpCommsActive tcp_test_node;
	tcp_test_node.initializeTest(nh);
	ros::spin();
	return 0;

}
