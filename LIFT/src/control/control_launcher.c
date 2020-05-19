/*
 * sender.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include <pthread.h>
#include <unistd.h>
#include "../common/fsm.h"
#include "control_includes.h"

//destination IP and port numbers
static const char DRONE_IP[] = "10.87.20.96";
static int TX_PORT = 9996;
static int RX_PORT = 9997;


static volatile int gCommunicationThread =0;

//for communication messages after STS
void *communication_handler_thread(void *argdata){
	PRINT_STATE()
	Memory *memory = (Memory *) argdata;
	uint16_t counter = 0;
	Timer myTimer;
	startTimer(&myTimer);
	make_command_request_packet(memory);
	gCommunicationThread=1;
	for(counter = 0; counter < MAX_COMMUNICATION_RETRANSMISSION; counter ++){
		if(!gCommunicationThread)return (void*)RETURN_SUCCESS;
		send_packet(memory->send_buf, memory->send_buf_len);  // send packet
		DEBUG_FSM("Communication packet sent\n")
		usleep(500000); //retransmit every 500ms to have a reliable communication
	}
	DEBUG_FSM("Retransmission counter exceeded, drone is unreachable")
	return (void*) DRONE_UNREACHABLE;
}


void *communication_receive_handler_thread(void *argdata){
	Memory *memory = (Memory *) argdata;
	uint32_t crc = 0;
	WORD_LEN response_len = {0};
	uint16_t timeoutSocket = 0;
	uint16_t valid = 0;
	WORD_TAG tag = TAG_UNDEFINED;
	WORD_LEN len = 0;
	uint16_t cmd=0;
	uint16_t cmd_type=0;
	uint32_t seq_num=0;

	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint8_t response[MAX_DATA_LENGTH] = {0};
	uint8_t response_data[MAX_DATA_LENGTH] = {0};

	Timer myTimer;
	startTimer(&myTimer);

	while(memory->current_state==CONTROL_SEND_COMMAND && valueTimer(&myTimer)<SESSION_TIMEOUT){
		initArray8(data, MAX_DATA_LENGTH);

		valid = receive_packet( &tag, &len,&crc, data, memory->receiverID, &timeoutSocket);
		if(timeoutSocket){
			// timeout of UDP socket
			//	printf("nothing recv\n");
		}
		else if(!valid){
			DEBUG_FSM("invalid id or incorrect integrity check")
		}
		else{
			DEBUG_FSM("valid id and correct integrity check")
			if(tag == TAG_COMMAND){

				if(DEBUG)printf("Command Response received crc=[%08x]",crc);
				valid = make_decrypted_message(response, &response_len, memory, data);
				if(valid){
					//decompose the response:
					decompose_command_response(response_data, &seq_num,  &cmd_type, &cmd, response, response_len);
					if(DEBUG)printf("\nSequence number: [%d] cmd_type [%d]  cmd [%d] \n", seq_num,cmd_type,cmd);
					switch (cmd_type){
					case SESSION_CONTROL_REQUEST:
						if((response_data[1]<<8|response_data[0])==SESSION_ACK||(response_data[1]<<8|response_data[0])==SESSION_REACK){
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
							if(cmd==DRONE_TURN_RIGHT)printf("DRONE TURNED RIGHT SUCCESSFULLY\n");
							else if(cmd==DRONE_TURN_LEFT)printf("DRONE TURNED LEFT SUCCESSFULLY\n");
							else if(cmd==DRONE_HIGHER)printf("DRONE HIGHER SUCCESSFULLY\n");
							else if(cmd==DRONE_LOWER)printf("DRONE LOWER SUCCESSFULLY\n");
						}
						break;
					case SESSION_STATUS_REQUEST_MESSAGE:
						if (cmd==DRONE_GET_GPS){
							printf("RECEIVED DRONE GPS COORDINATES: \n");
							printf("\tLONGITUDE: %02d.%02d%02d%02d \n",response_data[3],response_data[2],response_data[1],response_data[0]);
							printf("\tLATITUDE: %02d.%02d%02d%02d \n",response_data[7],response_data[6],response_data[5],response_data[4]);
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
						}
						else if (cmd==DRONE_GET_BATTERY){
							printf("RECEIVED DRONE BATTERY STATUS: \n");
							printf("\t\tBattery(12V): %2d %%  \n",response_data[0]);
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
						}
						else if(cmd==DRONE_GET_TEMPERATURE){
							printf("RECEIVED TEMPERATURE VALUE: \n");
							printf("\t Temperature: %2d Degrees  \n",response_data[0]);
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
						}
						else{
							return RETURN_INVALID; //do nothing, ignore the packet
						}
						break;
					case SESSION_VIDEO_STREAM_REQUEST:
						if(cmd==DRONE_VIDEO_FRAME){
							if((response_data[3]<<24|response_data[2]<<16|response_data[1]<<8|response_data[0]) > memory->vid_seq_num)
								memory->vid_seq_num+=1; //ideally this should be equal to the total frames sent from the drone but some packets can be dropped
						}
						else if((response_data[1]<<8|response_data[0])==SESSION_ACK||(response_data[1]<<8|response_data[0])==SESSION_REACK){
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
							printf("DRONE VIDEO STREAMING IN THE BACKGROUND \n");
						}
						break;
					case SESSION_TERMINATE_VIDEO_STREAM:    // OK message, this massage does not contain any data
						if((response_data[1]<<8|response_data[0])==SESSION_ACK||(response_data[1]<<8|response_data[0])==SESSION_REACK){
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
							memory->is_videostreaming=0;
							printf("DRONE VIDEO STREAMING STOPPED total received frames = [%d] [total %d Mbytes]\n",memory->vid_seq_num, (memory->vid_seq_num*VIDEOFRAMES*32/1000000));
						}
						break;
					default:
						printf("unknown command type (%s)\n", __func__);
					}
				}
			}
			else{
				printf("unexpected tag, message ignored crc=[%08x]\n",crc);
			}
		}
	}

	if (memory->current_state!=RESTART_SESSION) memory->current_state=SESSION_TIMEOUT_STATE;
	printf("Closing session \n");
	pthread_exit(NULL);

}


//for session handling
void session_handler(Memory *data){
	Memory *memory = (Memory *) data;
	int exit = 0;
	while(!exit){
		switch(memory->current_state){
		case NULL_STATE:
			printf("do nothing");
			usleep(100000);
			//printf("did nothing");
			break;
		case IDLE_CC:
			memory->current_state = idle_cc_handler(memory);
			break;
		case STS_MAKE_0:
			memory->current_state = sts_make_0_handler(memory);
			break;
		case STS_SEND_0:
			memory->current_state = sts_send_0_handler(memory);
			break;
		case STS_MAKE_2:
			memory->current_state = sts_make_2_handler(memory);
			break;
		case STS_SEND_2:
			memory->current_state = sts_send_2_handler(memory);
			break;
		case STATE_EXIT:
			printf("STS Complete\n");
			if(DEBUG)print_num(memory->session_key);
			exit = 1;
			break;
		default:
			printf("undefined STATE\n");
			exit = 1;
			break;
		}
	}
}


int main(void){
	pthread_t communication_handler;
	pthread_t communication_receive_handler;
	Memory memory;
	initMemory(&memory, FALSE);

	init_socket(DRONE_IP, TX_PORT, RX_PORT);

	int exit = 0;
	int choice=0; //session actions
	int sub_choice=0; //Sub Actions
	printf("=================================================================\n");
	printf("\t\t Welcome to LIFT Drone Control System \n");
	printf("=================================================================\n");
	// start_menu();
	while(!exit){
		if(memory.current_state==NULL_STATE){choice=start_menu();}
		else if(memory.current_state==CONTROL_SEND_COMMAND){choice=control_menu();}
		else if(memory.current_state==SESSION_TIMEOUT_STATE){
			printf("SESSION TIMEOUT\n");
			initMemory(&memory, FALSE);
			choice=start_menu();
			memory.current_state=RESTART_SESSION;
		}
		usleep(1000);
		switch(choice){
		case INIT_ESTABLISH_CONNECTION:
			if ( memory.current_state==NULL_STATE ||memory.current_state==RESTART_SESSION){
				memory.current_state=IDLE_CC;
				session_handler(&memory);
				memory.current_state=CONTROL_SEND_COMMAND;
				printf("Key Establishment Phase Successful \n");

				if ((STATE)memory.current_state==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}
				pthread_create(&communication_receive_handler, NULL, communication_receive_handler_thread, (void*)&memory);
				pthread_detach(communication_receive_handler);
				break;
			}
			printf("!!!Invalid input, please try again!!!\n");
			break;
		case START_COMMUNICATION_STATUS:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			status_menu();
			scanf("%d",&sub_choice);
			scanf("%*[^\n]");
			if (sub_choice==DRONE_GET_GPS || sub_choice==DRONE_GET_BATTERY || sub_choice==DRONE_GET_TEMPERATURE){
				memory.cmd=sub_choice;
				memory.cmd_type=SESSION_STATUS_REQUEST_MESSAGE;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				if ((STATE)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}
				break;
			}
			printf("!!!Invalid input, please try again!!!\n");
			break;
		case START_COMMUNICATION_COMMAND:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			commands_menu();
			scanf("%d",&sub_choice);
			scanf("%*[^\n]");
			if (sub_choice==DRONE_TURN_LEFT || sub_choice==DRONE_TURN_RIGHT || sub_choice==DRONE_HIGHER || sub_choice==DRONE_LOWER){
				memory.cmd=sub_choice;
				memory.cmd_type=SESSION_CONTROL_REQUEST;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				if ((STATE)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}

				break;
			}
			printf("!!!Invalid input, please try again!!!\n");
			break;
		case START_VIDEO_STREAM_COMMUNICATION:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			if(memory.is_videostreaming==0){
				memory.is_videostreaming=1;
				memory.cmd=NULL_COMMAND;
				memory.cmd_type=SESSION_VIDEO_STREAM_REQUEST;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				break;
			}
			printf("Already Streaming \n");
			break;
		case TERMINATE_VIDEO_STREAM_COMMUNICATION:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			if(memory.is_videostreaming==1){
				memory.is_videostreaming=0;
				memory.cmd=NULL_COMMAND;
				memory.cmd_type=SESSION_TERMINATE_VIDEO_STREAM;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				if ((STATE)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}
				break;
			}
			printf("Currently No Video Streaming \n");
			break;
		case REINIT_ESTABLISH_CONNECTION:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			initMemory(&memory, FALSE);
			memory.current_state=RESTART_SESSION;
			choice=INIT_ESTABLISH_CONNECTION;
			break;
		case TERMINATE_SYSTEM:
			exit=1;
			printf("\nShutdown System\nThank You!\n");
			printf("=================================================================\n");
			break;

		default:
			printf("!!!Invalid input, please try again!!!\n");
			break;
		}
	}

	close_sockets();
	return RETURN_SUCCESS;
}


