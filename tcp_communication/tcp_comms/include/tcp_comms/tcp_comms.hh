#pragma once
#ifndef tcp_COMMS
#define tcp_COMMS

#include <msg_encoding_utils/bytestream_utils.hh>
#include <smart_device_msgs/RackStatus.h>
#include <smart_device_msgs/LoadPort.h>
#include <payload_msgs/tcpStatus.h>

#include <ros/ros.h>
#include <ros/console.h>
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Bool.h>

#include <string>
#include <sstream>

using namespace std;

class tcpCommsPassive{
	ros::Subscriber tcp_querry, tcp_response_check, tcp_load_initialize_sub, tcp_unload_initialize_sub;
	ros::Publisher tcp_response,tcp_status_pub, tcp_querry_pub, tcp_err_status_pub;

	int state, actual_msg_length, state_flag, number_of_loadport, flag;
	std::string received_reading, data_buffer;
	bool transfer_status;
	smart_device_msgs::RackStatus rack_msg;
	smart_device_msgs::LoadPort passive_tcp_state;
	payload_msgs::tcpStatus tcp_state_passive;

public:
	tcpCommsPassive();
  	~tcpCommsPassive();
	void initialize(ros::NodeHandle& dh);
	void publishStringTotcp(const std_msgs::String);
	void publishStatusFromtcp(const std_msgs::Int8);
	void checkReceivedMsg(const std_msgs::String::ConstPtr&);
	void tcpCommsLoadingSequence(const smart_device_msgs::RackStatus);
	void LoadingPacketSequence(int);
	void UnloadingPacketSequence(int);
	void tcpCommsUnloadingSequence(const smart_device_msgs::RackStatus);
	void resetRegister();


};

class tcpCommsActive{
	ros::Subscriber tcp_test_querry, tcp_test_response_check, tcp_test_load_initialize_sub, tcp_test_unload_initialize_sub;
	ros::Publisher tcp_test_tresponse,tcp_test_status_pub, tcp_test_querry_pub, tcp_test_err_status_pub;

	int state_test, actual_msg_length_test, state_flag_test, set_q0_test, flag_test;
	std::string test_received_reading, test_data_buffer;
	bool transfer_status_test;
	smart_device_msgs::RackStatus rack_msg_active;
	std_msgs::Int8 active_tcp_state;
	payload_msgs::tcpStatus tcp_state_active;

public:
	tcpCommsActive();
  	~tcpCommsActive();
	void initializeTest(ros::NodeHandle& dh);
	void publishStringTotcpTest(const std_msgs::String);
	void publishStatusFromtcpTest(const std_msgs::Int8);
	void checkReceivedMsgTest(const std_msgs::String::ConstPtr&);
	void tcpCommsTestLoadingSequence(const std_msgs::Int8);//unloading for tcp
	void LoadingPacketSequenceTest(int);
	void tcpCommsTestUnloadingSequence(const std_msgs::Int8);//loading for tcp
	void UnloadingPacketSequenceTest(int);
	void resetRegisterTest();

};


#endif
