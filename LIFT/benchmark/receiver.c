#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "communication.h"


// number of messages to send
#define MSGS 50

int main(void)
{


	uint8_t key[CHACHA_KEY_LENGTH]={0};
	char2byte(key,"808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"); //hardcoding dummy key for encrypt-decrypt
	EntropyPool pool;
	initPool(&pool);

	int  recvlen;					// # bytes received
	uint8_t buf[MAX_TRANSFER_LENGTH];	// message buffer
	uint32_t seq_num=0;
	uint32_t rcvseq_num=0;
	uint16_t buf_len=0;

	init_socket(9999, 9998, 5);

	for (seq_num=0; seq_num<MSGS; seq_num++) {
		recvlen = receive_message(buf);
		buf[recvlen] = 0;
		if (IFENCRYPT)make_decryption(&rcvseq_num, key, buf);

		if (IFENCRYPT){
			initArray8(buf,MAX_TRANSFER_LENGTH);
			buf_len=make_encryption(buf, key, seq_num, &pool);
		}
		else{
			sprintf(buf, "This is the generated sequence number %d\n", seq_num);
			buf_len=strlen(buf);
		}
		if(send_message(buf, buf_len)==0) {
			close_sockets();
			printf("Send failed at message %d\n", seq_num);
			return 0;
		}
	}

	close_sockets();
	return 0;
}
