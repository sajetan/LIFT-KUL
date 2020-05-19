/*
 * receiver.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "../common/main.h"
#include "../common/fsm.h"
#include "drone_includes.h"

//destination IP and port numbers
static const char CC_IP[] = "10.87.20.93";
static int TX_PORT = 9997;
static int RX_PORT = 9996;


int main(void)
{
	// Time-out is set to 5 seconds
	init_socket(CC_IP, TX_PORT, RX_PORT);

	// FSM part
	Memory memory;
	initMemory(&memory, TRUE);
	int exit = 0;
	memory.current_state= IDLE_DRONE;

	while(!exit){
		switch(memory.current_state){
		case IDLE_DRONE:
			memory.current_state = idle_drone_handler(&memory);
			break;
		case STS_MAKE_1:
			memory.vid_seq_num=0;
			memory.current_state = sts_make_1_handler(&memory);
			break;
		case STS_SEND_1:
			memory.current_state = sts_send_1_handler(&memory);
			break;
		case STS_COMPLETED_DRONE:
			if(DEBUG)print_num(memory.session_key);
			memory.current_state = sts_completed_drone_handler(&memory);
			break;
		case STATE_EXIT:
			if(DEBUG)print_num(memory.session_key);
			exit = 1;
			break;
		default:
			printf("undefined STATE\n");
			exit = 1;
			break;
		}
	}	
	close_sockets();
	return 0;
}
