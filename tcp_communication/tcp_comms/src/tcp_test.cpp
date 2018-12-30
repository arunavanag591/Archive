#include <tcp_comms/tcp_Comms.hh>
//TODO:rename test to active
tcpCommsActive::tcpCommsActive(){
	actual_msg_length_test = 0;
	state_flag_test=0;
	flag_test=0;

}

tcpCommsActive::~tcpCommsActive(){ }


void tcpCommsActive::initializeTest(ros::NodeHandle& dh){

	tcp_test_load_initialize_sub = dh.subscribe("tcp_loading_inputs", 1, &tcpCommsActive::tcpCommsTestLoadingSequence, this);
	tcp_test_unload_initialize_sub = dh.subscribe("tcp_unloading_inputs", 1, &tcpCommsActive::tcpCommsTestUnloadingSequence, this); //trigger from agv
	tcp_test_response_check = dh.subscribe("ModbusTcpTest_node/read",10, &tcpCommsActive::checkReceivedMsgTest, this);//containing raw_msgs from tcp

	tcp_test_querry_pub = dh.advertise<std_msgs::String>("ModbusTcpTest_node/write", 10); // topic that will write to socket_comms
	tcp_test_status_pub = dh.advertise< std_msgs::Int8 > ("active_agv_tcp_status",10);//output of tcp->agv will listen to
	tcp_test_err_status_pub = dh.advertise< std_msgs::String > ("active_agv_tcp_err_status",1);//error from tcp->agv will listen to
}

void tcpCommsActive::publishStringTotcpTest(const std_msgs::String tcppackets){
	ros::Duration(0.2).sleep();
	tcp_test_querry_pub.publish(tcppackets);

}

void tcpCommsActive::publishStatusFromtcpTest(const std_msgs::Int8 statMsg){
	ros::Duration(0.2).sleep();
	tcp_test_status_pub.publish(statMsg);
}

void tcpCommsActive::checkReceivedMsgTest(const std_msgs::String::ConstPtr& msg){

	std_msgs::Int8 status_msg;
	ROS_INFO_THROTTLE(2,"Decoding Incoming Test tcp_Packets: %s", ToHex(msg->data, false).c_str());
	std::string s;

	if(msg->data.empty()){
		ROS_ERROR("No Message Received");
	}
	else{
	int buffer_index = 0, bytes_read = msg->data.length();
	std::string buffer = msg->data;
	state_test=0;
	while (bytes_read > 0) {
	    // Checking the modbus header
	    switch (state_test) {
	      case 0:

	        if (static_cast<unsigned char>(buffer[buffer_index]) == strtol(string("0x00").c_str(), NULL, 16)) {
	          state_test = 1;
	          test_data_buffer += buffer[buffer_index];
	        } else {
	          test_data_buffer.clear();
	          state_test = 0;
	        }
	        bytes_read--;
	        buffer_index++;
	        break;
	      case 1:

	        if (static_cast<unsigned char>(buffer[buffer_index]) == strtol(string("0x84").c_str(), NULL, 16)) {
	          state_test = 2;
	          test_data_buffer += buffer[buffer_index];
	        } else {
	          test_data_buffer.clear();
	          state_test = 0;
	        }
	        bytes_read--;
	        buffer_index++;
	        break;
	      case 2:

	        if (buffer[buffer_index] == 00) {
	          state_test = 3;
	          test_data_buffer += buffer[buffer_index];
	        } else {
	          test_data_buffer.clear();
	          state_test = 0;
	        }
	        bytes_read--;
	        buffer_index++;
	        break;
	      case 3:

	      	if (buffer[buffer_index] == 00) {
	      		state_test = 4;
	      		test_data_buffer += buffer[buffer_index];
	      	} else {
	      		test_data_buffer.clear();
	      	  state_test = 0;
	      	}
	      	bytes_read--;
	      	buffer_index++;
	      	break;
	      case 4:

	      	if (buffer[buffer_index] == 00) {
	      	   state_test = 5;
	      	   test_data_buffer += buffer[buffer_index];
	      	 } else {
	      	   test_data_buffer.clear();
	      	   state_test = 0;
	      	 }
	      	 bytes_read--;
	      	 buffer_index++;
	      	 break;
	      case 5:{//variable byte
	      	bytes_read--;

         	 actual_msg_length_test= buffer[buffer_index];
	      	 if(actual_msg_length_test == bytes_read){
	      		 state_test = 6;

	      		 test_data_buffer += buffer[buffer_index];
	      	 } else {
	      		 test_data_buffer.clear();
	      		 ROS_INFO("Failed Case");
	      		 state_test = 0;
	      	 }
	      	 bytes_read--;
	      	 buffer_index++;
	      	 }
	         break;
	      case 6:

	      	if (static_cast<unsigned char>(buffer[buffer_index]) == strtol(string("0x54").c_str(), NULL, 16)) {
	      	 state_test = 7;
	      	 test_data_buffer += buffer[buffer_index];
	      	 } else {
	      	 test_data_buffer.clear();
	      	 state_test = 0;
	      	 }
	      	 bytes_read--;
	      	 buffer_index++;
	      	 break;
	      	 //end of header
	      	 //next byte is the function code-8th byte
	      	 //next byte is the message length-9th byte
	      case 7:{
	      	test_data_buffer +=buffer.substr(buffer_index, msg->data.length());
	      	bytes_read=0;
	      	std::string loading_seq_status, unloading_seq_status, err_status;
	      	unsigned long loading_sequence;
	      	unsigned short unloading_sequence;
	      	unsigned short err_sequence;
	      	unsigned long function_sequence;
	      	int count_load = 0;
	      	int count_unload = 0;
	      	int count_err = 0;
	      	int count_func = 0;


	      	std::string test,test1,test2;
	      	char function_type=test_data_buffer[7];
	      	char operation_status=test_data_buffer[9];
	      	loading_seq_status=test_data_buffer[10];
	       	unloading_seq_status=test_data_buffer[10];
	      	err_status=test_data_buffer[14];
	      	bool function_status[16]={0};
	    		bool tcp_load_status[16] = {0};
	    		bool tcp_unload_status[16] = {0};
	    		bool tcp_error_status[16] = {false};
	    		loading_sequence=bytesTo<unsigned short>(loading_seq_status);
	    		unloading_sequence=bytesTo<unsigned short>(unloading_seq_status);
	    		err_sequence=bytesTo<unsigned short>(err_status);

	    		for(int i = 15; i >= 0; i--){
	    			tcp_load_status[i]=(loading_sequence & (1 << i) ? 1 : 0);
      		  tcp_unload_status[i]=(unloading_sequence & (1 << i) ? 1 : 0);
	      		tcp_error_status[i]=(err_sequence & (1 << i) ? 1 : 0);

	      	}
	      	for(int j = 0; j < 16; j++){
	      		if(tcp_load_status[j] & 1){++count_load;}
	       		if(tcp_unload_status[j] & 1){++count_unload;}
	      		if(tcp_error_status[j] & 1){++count_err;}

	      	}
	      	if(count_err >= 1){
	      		ROS_ERROR_THROTTLE(2,"UNEXPECTED ERROR PACKET");
	      		std_msgs::String err_type_pub;
						err_type_pub.data = ToHex(err_status,1);
						tcp_test_err_status_pub.publish(err_type_pub);
						resetRegisterTest();
						break;
	      	}
	      	 ROS_INFO("COUNT_LOAD outside: %d",count_load );
	      	ROS_INFO("COUNT_UNLOAD outside: %d",count_unload );

	      	if (operation_status == 1) {flag_test = 1;}
	      	else if (operation_status == 2){flag_test = 2;}

	      	if (flag_test == 1){
	      	if((count_load < 1 || count_load > 8) && function_type==3){
	      		status_msg.data = payload_msgs::tcpStatus::tcp_EMPTY_BYTES;
	      		test_data_buffer.clear();
	      		state_flag_test=1;
	      		publishStatusFromtcpTest(status_msg);
	      		LoadingPacketSequenceTest(state_flag_test);
	      		state_test =0;
	      	}
	      	else if((count_load < 1 || count_load > 8) && function_type == 5){
	      		test_data_buffer.clear();
	      		state_flag_test=10;
	      		LoadingPacketSequenceTest(state_flag_test);
	      		state_test=8;
	      	}
	      } else if (flag_test == 2){
	        if((count_unload < 1 || count_unload > 8) && function_type == 3){
	      		status_msg.data = payload_msgs::tcpStatus::tcp_EMPTY_BYTES;;
	      	  test_data_buffer.clear();
	      		state_flag_test=1;
	      		publishStatusFromtcpTest(status_msg);
	      		UnloadingPacketSequenceTest(state_flag_test);
	      		state_test =0;
	      	}
	      	else if((count_unload < 1 || count_unload > 8) && function_type == 5){
	      		test_data_buffer.clear();
	      		state_flag_test=10;
	      		UnloadingPacketSequenceTest(state_flag_test);
	      		state_test=8;
	      	}
	      }

	      	if(operation_status == 1 || flag_test == 1){
	      		ROS_INFO("Number of 1s in count_load = %d", count_load);
	      		ROS_INFO("LOADING SEQUENCE");
	      			if(count_load == 1) {
	      			status_msg.data = payload_msgs::tcpStatus::CONNECTION_ESTABLISHED;
	      			test_data_buffer.clear();
	      			state_flag_test=2;
	      			publishStatusFromtcpTest(status_msg);
	      			LoadingPacketSequenceTest(state_flag_test);
	      			state_test = 8;
	      	}
	      	else if(count_load == 2) {
	      			status_msg.data=payload_msgs::tcpStatus::LOAD_REQUEST_RECEIVED;
	      			test_data_buffer.clear();
	      			state_flag_test=3;
	      			publishStatusFromtcpTest(status_msg);
	      			LoadingPacketSequenceTest(state_flag_test);
	      	    state_test = 8;
	      	}
	      	else if(count_load == 3) {
	      			status_msg.data=payload_msgs::tcpStatus::TRANSFER_REQUEST_RECEIVED;
	      			test_data_buffer.clear();
	      			state_flag_test=4;
	      		  publishStatusFromtcpTest(status_msg);
	      			LoadingPacketSequenceTest(state_flag_test);
	      		  state_test = 8;
	      	}
	      	else if(count_load == 4) {
	      			status_msg.data=payload_msgs::tcpStatus::START_TRANSFER;
	      			test_data_buffer.clear();
	      			state_flag_test=5;
	      			publishStatusFromtcpTest(status_msg);
	      			LoadingPacketSequenceTest(state_flag_test);
	      			state_test = 8;

	      	}
	      	else if(count_load == 5) {
	      			status_msg.data=payload_msgs::tcpStatus::CARRIER_DETECTED;
	      			test_data_buffer.clear();
	      			state_flag_test=6;
	      			publishStatusFromtcpTest(status_msg);
	      			LoadingPacketSequenceTest(state_flag_test);
	      			state_test = 8;

	      	}
	      	else if(count_load == 6) {
	      		status_msg.data=payload_msgs::tcpStatus::AGV_COMMUNICATION_COMPLETE;
	      		test_data_buffer.clear();
	      		state_flag_test=7;
	      		publishStatusFromtcpTest(status_msg);
	      		LoadingPacketSequenceTest(state_flag_test);
	      		state_test = 8;

	      	}
	      	else if(count_load == 8) {
	      		status_msg.data=payload_msgs::tcpStatus::LOAD_SEQUENCE_INITIATED;
	      		state_flag_test=8;
	      		publishStatusFromtcpTest(status_msg);	test_data_buffer.clear();
	      		LoadingPacketSequenceTest(state_flag_test);
	      		state_test = 8;
	      	}
	      	else {
	      		test_data_buffer.clear();
	      	}
	      }
	      	else if(operation_status == 2 || flag_test == 2){

	      	ROS_INFO("Number of 1s in count_unload = %d", count_unload);
	      	ROS_INFO("UNLOADING SEQUENCE");
	      	if(count_unload == 1) {
	      	status_msg.data = payload_msgs::tcpStatus::CONNECTION_ESTABLISHED;
	      	test_data_buffer.clear();
	      	state_flag_test=2;
	      	publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      }	else if(count_unload == 2) {
	      	status_msg.data = payload_msgs::tcpStatus::LOAD_REQUEST_RECEIVED;
	      	test_data_buffer.clear();
	      	state_flag_test=3;
	      	publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      }	else if(count_unload == 3) {
	      	status_msg.data = payload_msgs::tcpStatus::TRANSFER_REQUEST_RECEIVED;
	      	test_data_buffer.clear();
	      	state_flag_test=4;
	      	publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      } else if(count_unload == 4) {
	      	status_msg.data = payload_msgs::tcpStatus::START_TRANSFER;
	      	test_data_buffer.clear();
	      	state_flag_test=5;
	      	publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;

	      }else if(count_unload == 5) {
	        status_msg.data = payload_msgs::tcpStatus::CARRIER_DETECTED;
	      	test_data_buffer.clear();
	      	state_flag_test=6;
	      	publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      }	else if(count_unload == 6) {
	      	status_msg.data = payload_msgs::tcpStatus::AGV_COMMUNICATION_COMPLETE;
	      	test_data_buffer.clear();
	      	state_flag_test=7;
	        publishStatusFromtcpTest(status_msg);
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      }	else if(count_unload == 8) {
	      	status_msg.data = payload_msgs::tcpStatus::LOAD_SEQUENCE_INITIATED;
	      	state_flag_test=8;
	        publishStatusFromtcpTest(status_msg);	test_data_buffer.clear();
	      	UnloadingPacketSequenceTest(state_flag_test);
	      	state_test = 8;
	      }	else {
	        test_data_buffer.clear();
	      }

	      }else {
	      	test_data_buffer.clear();
	      }

	      }

	    }
		}
	}
}

void tcpCommsActive::tcpCommsTestLoadingSequence(const std_msgs::Int8 msg) {//smart rack to agv
	active_tcp_state = msg;
	ROS_INFO("Initiating Loading Sequence");
	std_msgs::String tcp_unloading_packet;
	if (active_tcp_state.data == 0) {
		unsigned char unload_seq_init_arr []={0, 132, 0, 0, 0, 6, 84, 5, 0, 1, 255,	0 };
		std::string unload_init_data ( unload_seq_init_arr, unload_seq_init_arr + sizeof unload_seq_init_arr / sizeof unload_seq_init_arr[0]);
		tcp_unloading_packet.data=unload_init_data;
		ROS_INFO("Loading Sequence msg: size %lu,  %s", tcp_unloading_packet.data.size(), ToHex(tcp_unloading_packet.data, false).c_str());
		publishStringTotcpTest(tcp_unloading_packet);
	}

}

void tcpCommsActive::tcpCommsTestUnloadingSequence(const std_msgs::Int8 msg) { //agv to smart rack
	active_tcp_state = msg;

	ROS_INFO("Initiating UnLoading Sequence");
	std_msgs::String tcp_loading_packet;
	if (active_tcp_state.data == 0) {
		unsigned char load_seq_init_arr []={ 0, 132, 0, 0, 0, 6, 84, 5, 0, 2, 255,	0 };
		std::string load_init_data ( load_seq_init_arr, load_seq_init_arr + sizeof load_seq_init_arr / sizeof load_seq_init_arr[0]);
		tcp_loading_packet.data=load_init_data;
		ROS_INFO("UnLoading Sequence msg: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcpTest(tcp_loading_packet);
	}

}

void tcpCommsActive::resetRegisterTest(){
	std_msgs::String tcp_err_reset_packet, load_switch;
	unsigned char reset_packet []={0,132,0,0,0,6,84,5,0,3,255,0};
	std::string reset_packet_data ( reset_packet, reset_packet + sizeof reset_packet / sizeof reset_packet[0]);
	tcp_err_reset_packet.data=reset_packet_data;
	ROS_INFO_THROTTLE(2,"Reseting Registers: size %lu,  %s", tcp_err_reset_packet.data.size(), ToHex(tcp_err_reset_packet.data, false).c_str());
	publishStringTotcpTest(tcp_err_reset_packet);
}

void tcpCommsActive::LoadingPacketSequenceTest(int x){
	std_msgs::String tcp_loading_packet;
	state_flag_test=x;

	if (state_flag_test==1){
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,1,255,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2,"Loading Sequence msg->state_flag_test_1: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
	}

	if (state_flag_test==2){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_2: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==3){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_3: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==4){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_4: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==5){//CARRIER_DETECTED
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_5: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if(state_flag_test==6){//START_TRANSFER===The 1f state on active side.
			set_q0_test++;
			ROS_INFO("active_tcp_state  : %d", active_tcp_state.data);
			if (active_tcp_state.data == 1){ROS_INFO("inside smart_msg");
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,1,0,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_6-1: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);

			}
			else
			{
				ROS_INFO("inside read_msg");
				unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
				std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
				tcp_loading_packet.data=read_packet_data;
				ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_6-2: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
				publishStringTotcpTest(tcp_loading_packet);
			}
		}
	if (state_flag_test==7){//AGV_COMMUNICATION_COMPLETE
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,1,0,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_7: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==8){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2,"Loading Sequence msg->state_flag_test-8: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==10){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,1,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_10: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}

	//TODO: reset registers and exit
}

void tcpCommsActive::UnloadingPacketSequenceTest(int x){
	std_msgs::String tcp_loading_packet;
	state_flag_test=x;


	if (state_flag_test==1){
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,2,255,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2,"Loading Sequence msg->state_flag_test_1: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
	}

	if (state_flag_test==2){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_2: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==3){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_3: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==4){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_4: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==5){//CARRIER_DETECTED
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_5: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if(state_flag_test==6){//START_TRANSFER===The 1f state on active side.
			set_q0_test++;
			ROS_INFO("active_tcp_state  : %d", active_tcp_state.data);
			if (active_tcp_state.data == 1){ROS_INFO("inside smart_msg");
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,2,0,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_6-1: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);

			}
			else
			{
				ROS_INFO("inside read_msg");
				unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
				std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
				tcp_loading_packet.data=read_packet_data;
				ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_6-2: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
				publishStringTotcpTest(tcp_loading_packet);
			}
		}
	if (state_flag_test==7){//AGV_COMMUNICATION_COMPLETE
			unsigned char read_packet []={0,132,0,0,0,6,84,5,0,2,0,0};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_7: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==8){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2,"Loading Sequence msg->state_flag_test-8: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}
	if (state_flag_test==10){
			unsigned char read_packet []={0,132,0,0,0,6,84,3,0,2,0,3};
			std::string read_packet_data ( read_packet, read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data=read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_test_10: size %lu,  %s", tcp_loading_packet.data.size(), ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcpTest(tcp_loading_packet);
		}

	//TODO: reset registers and exit
}
