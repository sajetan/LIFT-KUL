
#include "main.h"

// number of messages to send
#define MSGS 5	

int main(void)
{
	int i;
	int  recvlen;					// # bytes received
	uint8_t buf[MAX_TRANSFER_LENGTH];	// message buffer
	uint16_t buf_len;	// message buffer length




	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(9998, 9999, 5); 
	
	// Ports for communication over GNU Radio
	// init_socket(21236, 21234, 5);
    
    // now let's send and receive the messages

	WORD_TAG tag = 0;
	WORD_ID len = 6;
	WORD_ID id = 0;
	uint8_t data[10] ="abcdefg";

	getTLV(buf, &buf_len, tag, len, id, data);

	if(send_message(buf, buf_len)==0) {
		close_sockets();
		printf("Send failed at message tag %d\n", tag);
		return 0;
	}
	printf("Sent:     %s\n", buf);
	for(int i = 0; i<buf_len; i++){
		printf("%x ", buf[i]);
	}

	/*
	for (i=0; i<MSGS; i++) {
		
        sprintf(buf, "Ping message %d to sender\n", i);
        if(send_message(buf, strlen(buf))==0) {
			close_sockets();
			printf("Send failed at message %d\n", i);
			return 0;
		}
		printf("Sent:     %s\n", buf);

		
		recvlen = receive_message(buf);
		if(recvlen == -1) {
			printf("Receive Failed: Timeout\n");
		}
		else {
			buf[recvlen] = 0;
			printf("Received: %s\n", buf);
		}
	}
	*/
	
	close_sockets();
	return 0;
}
