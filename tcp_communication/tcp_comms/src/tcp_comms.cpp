#include <tcp_comms/tcp_comms.hh>

tcpCommsPassive::tcpCommsPassive() {
	actual_msg_length = 0;
	state_flag = 0;
	flag = 0;
	rack_msg.ports.push_back(passive_tcp_state);
}

tcpCommsPassive::~tcpCommsPassive() {}

void tcpCommsPassive::initialize(ros::NodeHandle& dh) {
	tcp_load_initialize_sub = dh.subscribe("rack_log_to_server_topic", 1,&tcpCommsPassive::tcpCommsLoadingSequence, this); //this is unloading from agv point of view
	tcp_unload_initialize_sub = dh.subscribe("rack_log_to_server_topic", 1,&tcpCommsPassive::tcpCommsUnloadingSequence, this); //this is unloading from agv point of view
	tcp_response_check = dh.subscribe("ModbusTcpClient_node/read", 10, &tcpCommsPassive::checkReceivedMsg, this); //containing raw_msgs from tcp
	tcp_querry_pub = dh.advertise < std_msgs::String> ("ModbusTcpClient_node/write", 10); // topic that will write to socket_comms
	tcp_status_pub = dh.advertise < std_msgs::Int8 > ("passive_rack_tcp_status", 10); //output of tcp
	tcp_err_status_pub = dh.advertise < std_msgs::String > ("passive_rack_tcp_err_status", 1); //error from tcp->agv will listen to
}

void tcpCommsPassive::publishStringTotcp(const std_msgs::String tcppackets) {

	ros::Duration(0.4).sleep();
	tcp_querry_pub.publish(tcppackets);

}

void tcpCommsPassive::publishStatusFromtcp(const std_msgs::Int8 statMsg) {
	ros::Duration(0.3).sleep();
	tcp_status_pub.publish(statMsg);
}

void tcpCommsPassive::checkReceivedMsg(const std_msgs::String::ConstPtr& msg) {

	std_msgs::Int8 status_msg;
	ROS_INFO("Decoding Incoming tcp_Packets: %s",	ToHex(msg->data, false).c_str());
	std::string s;

	if (msg->data.empty()) {
		ROS_ERROR("No Message Received");
	} else {
		int buffer_index = 0, bytes_read = msg->data.length();
		std::string buffer = msg->data;
		state = 0;
		while (bytes_read > 0) {
			// Checking the modbus header
			switch (state) {
			case 0:

				if (static_cast<unsigned char>(buffer[buffer_index])
						== strtol(string("0x00").c_str(), NULL, 16)) {
					state = 1;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
			case 1:

				if (static_cast<unsigned char>(buffer[buffer_index])
						== strtol(string("0x84").c_str(), NULL, 16)) {
					state = 2;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
			case 2:

				if (buffer[buffer_index] == 00) {
					state = 3;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
			case 3:

				if (buffer[buffer_index] == 00) {
					state = 4;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
			case 4:

				if (buffer[buffer_index] == 00) {
					state = 5;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
			case 5: { //variable byte
				bytes_read--;

				actual_msg_length = buffer[buffer_index];
				if (actual_msg_length == bytes_read) {
					state = 6;

					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					ROS_INFO("failed case");
					state = 0;
				}
				bytes_read--;
				buffer_index++;
			}
				break;
			case 6:

				if (static_cast<unsigned char>(buffer[buffer_index])
						== strtol(string("0x54").c_str(), NULL, 16)) {
					state = 7;
					data_buffer += buffer[buffer_index];
				} else {
					data_buffer.clear();
					state = 0;
				}
				bytes_read--;
				buffer_index++;
				break;
				//end of header
				//next byte is the function code-8th byte
				//next byte is the message length-9th byte
			case 7: {
				data_buffer +=buffer.substr(buffer_index, msg->data.length());
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


				char function_type=data_buffer[7];
				char operation_status=data_buffer[9];

			  loading_seq_status=data_buffer[10];
      	unloading_seq_status=data_buffer[10];
      	err_status=data_buffer[14];
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

		   if (count_err >= 1) {
		    	ROS_ERROR_THROTTLE(2,"UNEXPECTED ERROR PACKET");
		    	std_msgs::String err_type_pub;
		    	err_type_pub.data = ToHex(err_status,1);
		    	tcp_err_status_pub.publish(err_type_pub);
		    	resetRegister();
		    	count_err=0;
		    	break;
				}

		   if (operation_status == 1) {flag = 1;}
			 else if (operation_status == 2){flag = 2;}

				if (flag == 1){

					if ((count_load < 1 || count_load > 8) && function_type == 3) {
						status_msg.data = payload_msgs::tcpStatus::tcp_EMPTY_BYTES;
						data_buffer.clear();
						state_flag = 1;
						publishStatusFromtcp(status_msg);
						LoadingPacketSequence(state_flag);
						state = 0;
			  	} else if ((count_load < 1 || count_load > 8) && function_type == 5) {
			  		data_buffer.clear();
			  		state_flag = 10;
			  		LoadingPacketSequence(state_flag);
			  		state = 8;
			  	}
				}else if (flag == 2){
					if ((count_unload < 1 || count_unload > 8) && function_type == 3) {
						status_msg.data = payload_msgs::tcpStatus::tcp_EMPTY_BYTES;
						data_buffer.clear();
						state_flag = 1;
						publishStatusFromtcp(status_msg);
						UnloadingPacketSequence(state_flag);
						state = 0;
					} else if ((count_unload < 1 || count_unload > 8) && function_type == 5) {
						data_buffer.clear();
						state_flag = 10;
						UnloadingPacketSequence(state_flag);
						state = 8;
					}
				}

				if(operation_status == 1 || flag == 1){
					ROS_INFO("Number of 1s in count_load = %d", count_load);
					ROS_INFO("LOADING SEQUENCE");
					if (count_load == 1) {
							status_msg.data =	payload_msgs::tcpStatus::CONNECTION_ESTABLISHED;
							data_buffer.clear();
							state_flag = 2;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;
				 } else if (count_load == 2) {
						  status_msg.data = payload_msgs::tcpStatus::LOAD_REQUEST_RECEIVED;
						 	data_buffer.clear();
							state_flag = 3;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;
					} else if (count_load == 3) {
							status_msg.data =	payload_msgs::tcpStatus::TRANSFER_REQUEST_RECEIVED;
							data_buffer.clear();
							state_flag = 4;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;
					} else if (count_load == 4) {
							status_msg.data = payload_msgs::tcpStatus::START_TRANSFER;
							data_buffer.clear();
							state_flag = 5;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;
					} else if (count_load == 5) {
							status_msg.data =	payload_msgs::tcpStatus::CARRIER_DETECTED;
							data_buffer.clear();
							state_flag = 6;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;

					} else if (count_load == 6) {
							status_msg.data =	payload_msgs::tcpStatus::AGV_COMMUNICATION_COMPLETE;
							data_buffer.clear();
							state_flag = 7;
							publishStatusFromtcp(status_msg);
							LoadingPacketSequence(state_flag);
							state = 8;

					} else if (count_load == 8) {
							status_msg.data =	payload_msgs::tcpStatus::LOAD_SEQUENCE_INITIATED;
							state_flag = 8;
							publishStatusFromtcp(status_msg);
							data_buffer.clear();
							LoadingPacketSequence(state_flag);
							state = 8;
					} else {
						data_buffer.clear();
				  }

				}
				else if(operation_status == 2 || flag == 2){
					ROS_INFO("Number of 1s in count_unload = %d", count_unload);
					ROS_INFO("UNLOADING SEQUENCE");
				   if (count_unload == 1) {
							status_msg.data = payload_msgs::tcpStatus::CONNECTION_ESTABLISHED;
							data_buffer.clear();
							state_flag = 2;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else if (count_unload == 2) {
							status_msg.data = payload_msgs::tcpStatus::LOAD_REQUEST_RECEIVED;
							data_buffer.clear();
							state_flag = 3;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else if (count_unload == 3) {
							status_msg.data = payload_msgs::tcpStatus::TRANSFER_REQUEST_RECEIVED;
							data_buffer.clear();
							state_flag = 4;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else if (count_unload == 4) {	    		//topic for->agv>>transfer_status
							status_msg.data = payload_msgs::tcpStatus::START_TRANSFER;
							data_buffer.clear();
							state_flag = 5;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else if (count_unload == 5) {	    	//topic for->smart~rack>>rack_status
							status_msg.data = payload_msgs::tcpStatus::CARRIER_DETECTED;
							data_buffer.clear();
							state_flag = 6;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
   				} else if (count_unload == 6) {	    		//another topic as final
							status_msg.data = payload_msgs::tcpStatus::AGV_COMMUNICATION_COMPLETE;
							data_buffer.clear();
							state_flag = 7;
							publishStatusFromtcp(status_msg);
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else if (count_unload == 8) {
							status_msg.data =	payload_msgs::tcpStatus::LOAD_SEQUENCE_INITIATED;
							data_buffer.clear();
							state_flag = 8;
							publishStatusFromtcp(status_msg);
							data_buffer.clear();
							UnloadingPacketSequence(state_flag);
							state = 8;
					} else {
							data_buffer.clear();
					}

				 }else {data_buffer.clear();}

				}
			}
		}
	}
}
void tcpCommsPassive::resetRegister() {
	std_msgs::String tcp_err_reset_packet, load_switch;
	unsigned char reset_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 3, 255, 0 };
	std::string reset_packet_data(reset_packet,
			reset_packet + sizeof reset_packet / sizeof reset_packet[0]);
	tcp_err_reset_packet.data = reset_packet_data;
	ROS_INFO_THROTTLE(2, "Reseting Registers: size %lu,  %s",	tcp_err_reset_packet.data.size(),	ToHex(tcp_err_reset_packet.data, false).c_str());
	publishStringTotcp(tcp_err_reset_packet);
}

void tcpCommsPassive::tcpCommsLoadingSequence(const smart_device_msgs::RackStatus msg1) {
	rack_msg = msg1;//unloading_ready: smart occupied, user pressed button > state=4
  ROS_INFO("Initiating Loading Sequence");
	std_msgs::String tcp_loading_packet;
	if (rack_msg.ports[0].state == smart_device_msgs::LoadPort::UNLOADING_READY) {
		unsigned char load_seq_init_arr[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 1, 255,	0 };
		std::string load_init_data(load_seq_init_arr,	load_seq_init_arr	+ sizeof load_seq_init_arr / sizeof load_seq_init_arr[0]);
		tcp_loading_packet.data = load_init_data;
		ROS_INFO("Loading Sequence msg: size %lu,  %s",	tcp_loading_packet.data.size(),ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}

}

void tcpCommsPassive::tcpCommsUnloadingSequence(const smart_device_msgs::RackStatus msg2) {
	rack_msg = msg2;	//loading ready (msg from log to server) :smart rack empty > state =1
	ROS_INFO("Initiating Unloading Sequence");
	std_msgs::String tcp_unloading_packet;
	if (rack_msg.ports[0].state == smart_device_msgs::LoadPort::LOADING_READY) {
		unsigned char unload_seq_init_arr[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 2, 255,	0 };
		std::string unload_init_data(unload_seq_init_arr,	unload_seq_init_arr	+ sizeof unload_seq_init_arr / sizeof unload_seq_init_arr[0]);
		tcp_unloading_packet.data = unload_init_data;
		ROS_INFO("Unloading Sequence msg: size %lu,  %s",	tcp_unloading_packet.data.size(),	ToHex(tcp_unloading_packet.data, false).c_str());
		publishStringTotcp(tcp_unloading_packet);
	}

}


void tcpCommsPassive::LoadingPacketSequence(int x) {
	std_msgs::String tcp_loading_packet;
	state_flag = x;
	ROS_INFO("state flag %d", state_flag);

	if (state_flag == 1) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 1, 255, 0 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_1: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}

	if (state_flag == 2) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_2: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 3) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_3: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 4) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_4: size %lu,  %s",	tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}

	if (state_flag == 5) {//START_TRANSFER

		ROS_INFO("passive_tcp_state : %d", rack_msg.ports[0].state);
		if (rack_msg.ports[0].state == smart_device_msgs::LoadPort::UNLOADING_SUCCESSFUL) {
			ROS_INFO("inside smart_msg");
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 1, 0, 0 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_5: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);

		} else {
			ROS_INFO("inside read_msg");
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2,
					"Loading Sequence msg->state_flag_6-1: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}
	}
	if (state_flag == 6) {//CARRIER_DETECTED
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_6-2: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 7) {//AGV_COMMUNICATION_COMPLETE -> TODO: have a ros time out , need to check why using function 5 and not 3 (does that matter)
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 1, 0, 0 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_7: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 8) {
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag-8: size %lu,  %s",	tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}

	if (state_flag == 10) {
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 1, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Loading Sequence msg->state_flag_4: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}

}

void tcpCommsPassive::UnloadingPacketSequence(int x) { //TODO: need to change this according to the state machine above for unloading
	std_msgs::String tcp_loading_packet;
	state_flag = x;
	ROS_INFO("state flag %d", state_flag);

	if (state_flag == 1) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 2, 255, 0 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_1: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}

	if (state_flag == 2) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_2: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 3) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_3: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 4) {
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_4: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}

	if (state_flag == 5) {//START_TRANSFER

		ROS_INFO("passive_tcp_state state : %d", rack_msg.ports[0].state);
		if (rack_msg.ports[0].state == smart_device_msgs::LoadPort::LOADING_SUCCESSFUL) {
			ROS_INFO("inside smart_msg");
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 2, 0, 0 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_5: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);

		} else {
			ROS_INFO("inside read_msg");
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2,"Unloading Sequence msg->state_flag_6-1: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}
	}
	if (state_flag == 6) {//CARRIER_DETECTED
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_6-2: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 7) {//AGV_COMMUNICATION_COMPLETE -> TODO: have a ros time out , need to check why using function 5 and not 3 (does that matter)
		unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 5, 0, 2, 0, 0 };
		std::string read_packet_data(read_packet,
				read_packet + sizeof read_packet / sizeof read_packet[0]);
		tcp_loading_packet.data = read_packet_data;
		ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_7: size %lu,  %s",
				tcp_loading_packet.data.size(),
				ToHex(tcp_loading_packet.data, false).c_str());
		publishStringTotcp(tcp_loading_packet);
	}
	if (state_flag == 8) {
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag-8: size %lu,  %s",
					tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}

	if (state_flag == 10) {
			unsigned char read_packet[] = { 0, 132, 0, 0, 0, 6, 84, 3, 0, 2, 0, 3 };
			std::string read_packet_data(read_packet,
					read_packet + sizeof read_packet / sizeof read_packet[0]);
			tcp_loading_packet.data = read_packet_data;
			ROS_INFO_THROTTLE(2, "Unloading Sequence msg->state_flag_10: size %lu,  %s",	tcp_loading_packet.data.size(),
					ToHex(tcp_loading_packet.data, false).c_str());
			publishStringTotcp(tcp_loading_packet);
		}

}

