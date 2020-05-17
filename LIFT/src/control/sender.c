/*
 * sender.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "../common/fsm.h"
#include <pthread.h>
#include <unistd.h>

static volatile int gCommunicationThread =0;
static volatile int gCommunicationCommandCheck =0;
static volatile int gCommunicationStatusCheck =0;
static volatile int gCommunicationVideoStreamcheck =0;

static volatile State gFSMState = NULL_STATE; //change this maybe


int start_menu(){
	int choice=0;
	printf("-----------------------------------------------------------------\n");
	printf("Press (1) to establish Drone Connection\n");
	printf("Press (9) to Shutdown System\n");
	printf("Please enter your choice here: ");
	fflush(stdin);
	(void)scanf(" %d",&choice);
	(void)scanf("%*[^\n]");
	fflush(stdin);
	return choice;
}

int control_menu(){
	int choice=0;
	printf("-----------------------------------------------------------------\n");
	printf("Press (2) to Get Drone Status\n");
	printf("Press (3) to Send Command to Drone\n");
	printf("Press (4) to Start Stream Video from Drone\n");
	printf("Press (5) to Terminate Stream Video from Drone\n");
	printf("Press (6) to RESTART Session\n");
	printf("Press (9) to Shutdown System\n");
	printf("Please enter your choice here: ");
	(void)scanf(" %d",&choice);
	(void)scanf("%*[^\n]");
	return choice;
}

void commands_menu(){
	printf("-----------------------------------------------------------------\n");
	printf("\nBelow are the control options:\n");
	printf("Press (1) to turn the Drone RIGHT\n");
	printf("Press (2) to turn the Drone LEFT\n");
	printf("Press (3) to higher altitude of Drone by 1m\n");
	printf("Press (4) to lower altitude of Drone by 1m\n");
	printf("Please enter your choice here: ");
}

void status_menu(){
	printf("-----------------------------------------------------------------\n");
	printf("\nBelow are the status request options:\n");
	printf("Press (1) to get GPS coordinates of Drone\n");
	printf("Press (2) to get Battery level of Drone\n");
	printf("Press (3) to get Temperature value\n");
	printf("Please enter your choice here: ");
}


//for session handling


//for user interaction
void *communication_handler_thread(void *argdata){
	//printf("communication_handler---here\n");
	//	int exit=0;

	Memory *memory = (Memory *) argdata;
	PRINT_STATE()
	//    uint16_t valid = 0;
	//    WORD_TAG tag = TAG_UNDEFINED;
	//    WORD_LEN len = 0;
	//    uint8_t data[MAX_DATA_LENGTH] = {0};
	//    uint16_t timeoutSocket = 0;
	uint16_t counter = 0;
	Timer myTimer;
	startTimer(&myTimer);
	make_command_request_packet(memory);
	//print_num(memory->session_key);
	gCommunicationThread=1;
	for(counter = 0; counter < MAX_COMMUNICATION_RETRANSMISSION; counter ++){
		//printf("inside forloop in communication handler \n");
		//    	print_num(memory->send_buf);
		if(!gCommunicationThread)return (void*)RETURN_SUCCESS;
		send_packet(memory->send_buf, memory->send_buf_len);  // send packet
		DEBUG_FSM("Communication packet sent\n")
		//startTimer(&myTimer);                           // start timer
		//while(valueTimer(&myTimer)<COMMUNICATION_TIMEOUT && gCommunicationThread){
			usleep(500000);
		//}
		//DEBUG_FSM("timeout,ack not received,retransmit packet\n")
	}
	DEBUG_FSM("timeout and retransmission counter exceeded, drone unreachable")
	return (void*) DRONE_UNREACHABLE;
}


void *communication_receive_handler_thread(void *argdata){
	//gCommunicationVideoStreamcheck=1;
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

		//printf("\tinside recv while ---------\n");
		//usleep(1000000);
		valid = receive_packet( &tag, &len,&crc, data, memory->receiverID, &timeoutSocket);
		if(timeoutSocket){
			// timeout of UDP socket
			//	printf("nothing recv\n");
		}
		else if(!valid){
			DEBUG_FSM("invalid id or incorrect integrity check")
		}
		else{
			//DEBUG_FSM("valid id and correct integrity check")
			if(tag == TAG_COMMAND){
				//printf("-----------------------------------------------------------------\n");
				//printf("Command Response received crc=[%08x]",crc);
				valid = make_decrypted_message(response, &response_len, memory, data);
				if(valid){
					//decompose the response:
					decompose_command_response(response_data, &seq_num,  &cmd_type, &cmd, response, response_len);
					//printf("\nsequence number: [%d] cmd_type [%d]  cmd [%d] \n", seq_num,cmd_type,cmd);
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
								memory->vid_seq_num+=1; //(uint32_t)(response_data[3]<<24|response_data[2]<<16|response_data[1]<<8|response_data[0]);
							//uint32_t frame_len = response_len-sizeof(lift_cmd_pkt);
							//PRINT_VIDEO_FRAME(memory->vid_seq_num,frame_len);
							//                        		if ((65535 < memory->vid_seq_num)&& (memory->vid_seq_num < 65537)){ //printf("frame numer %lu \n",(uint32_t)memory->vid_seq_num);
							//                        		printf("%08x %08x %08x %08x \n",(response_data[3],response_data[2],response_data[1],response_data[0]));
							//                        		printf("video frame -\n[");
							//                        		for(int i=0;i<(response_len);i++)
							//                        			printf("0x%02x, ",response_data[i]);
							//                        		printf("]\n");
							//                        		}


						}
						else if((response_data[1]<<8|response_data[0])==SESSION_ACK||(response_data[1]<<8|response_data[0])==SESSION_REACK){
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
							printf("DRONE VIDEO STREAMING IN THE BACKGROUND \n");
						}
						break;
					case SESSION_TERMINATE_VIDEO_STREAM:    // OK message, this massage does not contain any data
						if((response_data[1]<<8|response_data[0])==SESSION_ACK||(response_data[1]<<8|response_data[0])==SESSION_REACK){
							gCommunicationThread=0; //ack or re-ack received, terminate communication thread
							//gCommunicationVideoStreamcheck=0;
							memory->is_videostreaming=0;
							printf("DRONE VIDEO STREAMING STOPPED total received frames = [%d]\n",memory->vid_seq_num);
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
	pthread_exit(NULL);

}


//for session handling
//void *session_handler_thread(void *data){
void session_handler_thread(Memory *data){
	//pthread_exit(NULL);
	// Ports for direct communication
	// Time-out is set to 5 seconds
	//init_socket(9992, 9993, 10);

	// Ports for communication over GNU Radio
	//init_socket(21236, 21234, 5);

	// FSM part
	//	State state = idle_CC;
	Memory *memory = (Memory *) data;

	//print_num(memory->G.x);
	//initMemory(&memory); //, "123", "123");
	int exit = 0;
	//gFSMState = NULL_STATE;
	// udp part
	//uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer

	while(!exit){
		switch(memory->current_state){
		case NULL_STATE:
			printf("do nothing");
			usleep(100000);
			//exit=1;
			//printf("did nothing");
			//gFSMState=idle_CC;
			break;
		case idle_CC:
			memory->current_state = idle_CC_fct(memory);
			break;
		case key_exchange_CC:
			memory->current_state = key_exchange_CC_fct(memory);
			break;
		case STS_make_0:
			memory->current_state = STS_make_0_fct(memory);
			break;
		case STS_send_0:
			memory->current_state = STS_send_0_fct(memory);
			break;
		case STS_make_2:
			memory->current_state = STS_make_2_fct(memory);
			break;
		case STS_send_2:
			memory->current_state = STS_send_2_fct(memory);
			break;
		case State_Exit:
			printf("STS Complete\n");
			//print_num(memory->session_key);
			exit = 1;
			break;
		default:
			printf("undefined state\n");
			exit = 1;
			break;
		}
	}

}


int main(void){
	pthread_t communication_handler;
	pthread_t communication_receive_handler;
	Memory memory;
	initMemory(&memory); //, "123", "123");

	char *drone_ip = "127.0.0.1";

	init_socket(drone_ip, 9992, 9993, 10);
	int exit = 0;

	//SESSION_ACTIONS choice=0;
	int choice=0;
	int sub_choice=0;
	printf("=================================================================\n");
	printf("\t\t Welcome to LIFT Drone Control System \n");
	printf("=================================================================\n");
	// start_menu();
	while(!exit){

		//    	fflush(stdin);
		if(memory.current_state==NULL_STATE) {choice=start_menu();}
		else if(memory.current_state==CONTROL_SEND_COMMAND){choice=control_menu();}
		if (memory.current_state!=RESTART_SESSION){
			//    		fflush(stdin);
			//    		char tmp[5000];
			//    		fgets(tmp, 5000, stdin);
			//    		sscanf(tmp, "%d", &choice);

			//			scanf("%d",&choice);
			//			scanf("%*[^\n]");

		}

		usleep(1000);
		switch(choice){
		case INIT_ESTABLISH_CONNECTION:
			if ( memory.current_state==NULL_STATE ||memory.current_state==RESTART_SESSION){

				memory.current_state=idle_CC;
				session_handler_thread(&memory);
				memory.current_state=CONTROL_SEND_COMMAND;
				printf("Key Establishment Phase Successful \n");

				if ((State)memory.current_state==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");gFSMState=NULL_STATE;break;}
				pthread_create(&communication_receive_handler, NULL, communication_receive_handler_thread, (void*)&memory);
				pthread_detach(communication_receive_handler);
				break;
			}
			printf("!!!Invalid input, please try again!!!\n");
			break;
#if 1
		case START_COMMUNICATION_STATUS:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			status_menu();
			//    		fgets(c, 1, stdin);
			//    		sub_choice = atoi(c);
			//fflush(stdin);
			scanf("%d",&sub_choice);
			scanf("%*[^\n]");
			if (sub_choice==DRONE_GET_GPS || sub_choice==DRONE_GET_BATTERY || sub_choice==DRONE_GET_TEMPERATURE){
				memory.cmd=sub_choice;
				memory.cmd_type=SESSION_STATUS_REQUEST_MESSAGE;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				if ((State)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}
				break;
			}
			printf("!!!Invalid input, please try again!!!\n");
			break;
		case START_COMMUNICATION_COMMAND:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			commands_menu();
			//    		fgets(c, 1, stdin);
			//    		sub_choice = atoi(c);
			//fflush(stdin);
			scanf("%d",&sub_choice);
			scanf("%*[^\n]");
			if (sub_choice==DRONE_TURN_LEFT || sub_choice==DRONE_TURN_RIGHT || sub_choice==DRONE_HIGHER || sub_choice==DRONE_LOWER){
				memory.cmd=sub_choice;
				memory.cmd_type=SESSION_CONTROL_REQUEST;
				void *status = 0;
				pthread_create(&communication_handler, NULL, communication_handler_thread, (void*)&memory);
				pthread_join(communication_handler, &status);
				if ((State)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}

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
				if ((State)status==DRONE_UNREACHABLE){printf("\n!!!Drone unreachable! Try connecting again\n");memory.current_state=NULL_STATE;break;}
				break;
			}
			printf("Currently No Video Streaming \n");
			break;
		case REINIT_ESTABLISH_CONNECTION:
			if(memory.current_state!=CONTROL_SEND_COMMAND){printf("!!!Invalid input, please try again!!!\n");break;}
			initMemory(&memory);
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
#endif

		}

		//    	fflush(stdin);
		//    	fflush(stdout);
	}

	close_sockets();
	return (0);


}


