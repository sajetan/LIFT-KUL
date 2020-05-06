
#include "main.h"



int main(void)
{

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9993, 9992, 10);

	// Ports for communication over GNU Radio
	// init_socket(21235, 21237, 5);
    
	// FSM part
	State state = idle_drone;
	Memory memory;
	initMemory(&memory);
	int exit = 0;

	// udp part
	uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer
	uint16_t buf_len = 0;	// message buffer length
	

	while(!exit){
		switch(state){
			case idle_drone: 
				state = idle_drone_fct(&memory);
				break;
			case key_exchange_drone: 
				state = key_exchange_drone_fct(&memory);
				break;
			case STS_receive_0: 
				state = STS_receive_0_fct(&memory);
				break;
			case STS_make_1: 
				state = STS_make_1_fct(&memory);
				break;
			case STS_send_1: 
				state = STS_send_1_fct(&memory);
				break;
			case STS_receive_2: 
				state = STS_receive_2_fct(&memory);
				break;	
			case STS_send_OK: 
				state = STS_send_OK_fct(&memory);
				break;	
			case State_Exit: 
				exit = 1;
				break;
			default:
				printf("undefined state\n");
				exit = 1;
				break;
		}
	}	
	close_sockets();
	return 0;
}
