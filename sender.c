
#include "fsm.h"


int main(void)
{

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9992, 9993, 10); 
	
	// Ports for communication over GNU Radio
	// init_socket(21236, 21234, 5);

	// FSM part
	State state = idle_CC;
	Memory memory;
	initMemory(&memory); //, "123", "123");
	int exit = 0;

	// udp part
	uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer
	
	while(!exit){
		switch(state){
			case idle_CC: 
				state = idle_CC_fct(&memory);
				break;
			case key_exchange_CC: 
				state = key_exchange_CC_fct(&memory);
				break;
			case STS_make_0: 
				state = STS_make_0_fct(&memory);
				break;
			case STS_send_0: 
				state = STS_send_0_fct(&memory);
				break;
			case STS_receive_1: 
				state = STS_receive_1_fct(&memory);
				break;
			case STS_make_2: 
				state = STS_make_2_fct(&memory);
				break;
			case STS_send_2: 
				state = STS_send_2_fct(&memory);
				break;	
			case STS_receive_OK: 
				state = STS_receive_OK_fct(&memory);
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

