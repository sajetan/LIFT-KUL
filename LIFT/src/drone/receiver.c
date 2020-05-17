
#include "../common/main.h"
#include "../common/fsm.h"


int main(void)
{

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9993, 9992, 10);

	// Ports for communication over GNU Radio
	// init_socket(21235, 21237, 5);

	// FSM part
	Memory memory;
	initMemory(&memory);
	int exit = 0;
	memory.current_state= idle_drone;

	// udp part
	//	uint8_t buf[MAX_TRANSFER_LENGTH] = {0};	// message buffer
	//	uint16_t buf_len = 0;	// message buffer length


	while(!exit){
		switch(memory.current_state){
		case idle_drone:
			memory.current_state = idle_drone_fct(&memory);
			break;
		case key_exchange_drone:
			memory.current_state = key_exchange_drone_fct(&memory);
			break;
		case STS_make_1:
			memory.vid_seq_num=0;
			memory.current_state = STS_make_1_fct(&memory);
			break;
		case STS_send_1:
			memory.current_state = STS_send_1_fct(&memory);
			break;
		case STS_completed_drone:
			//				print_num(memory.session_key);
			memory.current_state = STS_completed_drone_fct(&memory);
			break;
		case State_Exit:
			//				print_num(memory.session_key);
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
