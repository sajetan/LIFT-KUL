
#include "main.h"

// number of messages to send
#define MSGS 5

int main(void)
{

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9999, 9998, 10);

	// Ports for communication over GNU Radio
	// init_socket(21235, 21237, 5);
    
	// FSM part
	State state = STS_send_1;
	Memory memory;
	initMemory(&memory);
	int exit = 0;

	// udp part
	uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer
	uint16_t buf_len = 0;	// message buffer length
	

	while(!exit){
		switch(state){
			case STS_send_1: 
				state = STS_send_1_fct(buf, &buf_len, &memory);
				break;
			case STS_send_OK: 
				state = STS_send_OK_fct(buf, &buf_len, &memory);
				break;	
			case STS_drone_completed: 
				state = STS_drone_completed_fct();
 				break;
			case State_Exit: 
				exit = 1;
				break;
		}
	}	
	close_sockets();
	return 0;
}
