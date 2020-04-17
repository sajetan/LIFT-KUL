
#include "main.h"

// number of messages to send
#define MSGS 5

int main(void)
{
	int i;
	int  recvlen;					// # bytes received
	uint8_t buf[MAX_TRANSFER_LENGTH];	// message buffer

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9999, 9998, 1000);

	// Ports for communication over GNU Radio
	// init_socket(21235, 21237, 5);
    
    // now let's send and receive the messages
	recvlen = receive_message(buf);
			buf[recvlen] = 0;
		printf("Received: %s\n", buf);

	for(int i = 0; i<recvlen; i++){
		printf("%d", buf[i]);
	}

	/*
	for (i=0; i<MSGS; i++) {
		recvlen = receive_message(buf);
		buf[recvlen] = 0;
		printf("Received: %s\n", buf);

		sprintf(buf, "Pong message %d from receiver\n", i);
        if(send_message(buf, strlen(buf))==0) {
			close_sockets();
			printf("Send failed at message %d\n", i);
			return 0;
		}
		printf("Sent:     %s\n", buf);
	}
	*/
	close_sockets();
	return 0;
}
