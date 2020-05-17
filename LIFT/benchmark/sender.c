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

//	char *drone_ip = "10.87.20.96";
	char *drone_ip = "127.0.0.1";
	init_socket(drone_ip, 9996, 9997, 10);

//	P(1)
	Timer t;
//	clock_t start =clock();
	startTimer(&t);
//	P(2)
	for (seq_num=0; seq_num< MSGS; seq_num++) {
		//startTimer(&t);
		//clock_t begin = clock();
//		P(4)
		buf_len=make_encryption(buf, key, seq_num, &pool);
//		P(5)
//		else{
//			sprintf(buf, "This is the generated sequence number %d\n", seq_num);
//			buf_len=strlen(buf);
//		}
//		P(6)
		if(send_message(buf, buf_len)==0) {
			close_sockets();
			printf("Send failed at message %d\n", seq_num);
			return 0;
		}
//		P(7)
		initArray8(buf,MAX_TRANSFER_LENGTH);
		recvlen = receive_message(buf);
		if(recvlen == -1) {
			printf("Receive Failed: Timeout\n");
		}
		else {
			buf[recvlen] = 0;
			make_decryption(&rcvseq_num, key, buf);
			//printTimer(&t);
			
			
			//clock_t end = clock();
			//double time_spent = (double)(end - begin) / (double)CLOCKS_PER_SEC;
			//printf("total time %f microseconds\n", time_spent*100);
			printf("Received: This is the generated sequence number %d\n\n", rcvseq_num);
			//else printf("Received : %s\n", buf);
		}
	}
	printTimer(&t);
//	clock_t finish=clock();
//	double total_time = (double)(finish-start ) / CLOCKS_PER_SEC;
//        printf("total time %f seconds throughput= %f\n", total_time, MSGS/total_time);

	close_sockets();
	return 0;
}
