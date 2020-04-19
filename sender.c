
#include "main.h"

// number of messages to send
#define MSGS 5	

int main(void)
{

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9998, 9999, 10); 
	
	// Ports for communication over GNU Radio
	// init_socket(21236, 21234, 5);

	// FSM part
	State state = STS_send_0;
	Memory memory;
	initMemory(&memory);
	int exit = 0;

	// udp part
	uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer
	uint16_t buf_len = 0;	// message buffer length
	

	while(!exit){
		switch(state){
			case STS_send_0: 
				state = STS_send_0_fct(buf, &buf_len, &memory);
				break;
			case STS_send_2: 
				state = STS_send_2_fct(buf, &buf_len, &memory);
				break;
			case STS_CC_completed: 
				state = STS_CC_completed_fct();
				break;
			case State_Exit: 
				exit = 1;
				break;
		}
	}
    
	
	close_sockets();
	return 0;
}