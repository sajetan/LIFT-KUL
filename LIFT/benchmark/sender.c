/*
 * sender.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "communication.h"

int main(void)
{
	uint8_t key[CHACHA_KEY_LENGTH]={0};
	EntropyPool pool;
	initPool(&pool);
	char2byte(key,"808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"); //hardcoding dummy key for encrypt-decrypt

	uint32_t seq_num;
	uint32_t rcvseq_num;
	int  recvlen;					// # bytes received
	uint8_t buf[MAX_TRANSFER_LENGTH];	// message buffer
	int buf_len=0;
	init_socket(9998, 9999, 5); 

	for (seq_num=0; seq_num< MSGS; seq_num++) {
		clock_t begin = clock();
		if (IFENCRYPT) buf_len=make_encryption(buf, key, seq_num, &pool);
		else{
			sprintf(buf, "This is the generated sequence number %d\n", seq_num);
			buf_len=strlen(buf);
		}

		if(send_message(buf, buf_len)==0) {
			close_sockets();
			printf("Send failed at message %d\n", seq_num);
			return 0;
		}
		initArray8(buf,MAX_TRANSFER_LENGTH);
		recvlen = receive_message(buf);
		if(recvlen == -1) {
			printf("Receive Failed: Timeout\n");
		}
		else {
			buf[recvlen] = 0;
			if (IFENCRYPT)make_decryption(&rcvseq_num, key, buf);
			clock_t end = clock();
			double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("total time %f seconds\n", time_spent);
			if (IFENCRYPT)printf("Received: This is the generated sequence number %d\n\n", rcvseq_num);
			else printf("Received : %s\n", buf);
		}
	}

	close_sockets();
	return 0;
}
