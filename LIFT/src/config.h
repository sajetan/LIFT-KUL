/*
 * config.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define DEBUG 0 //enable to print logs
#define P(print)    printf("hello %x \n", print);

#define PRINT_STATE()   if(1)\
		printf("[ Current STATE:  %s ]\n", __func__);
#define DEBUG_FSM(s)    if(0)\
		printf("\t%s \n",s);
#define PRINT_VIDEO_FRAME(s,l)    if(1)\
		printf("\tVideo Seq num [%d] len [%d] \n",s,l);

#define DEBUG_SIGNATURE 0
#define PRINT_CONTENT_UDP_PACKET 0

#define MAX_TRANSFER_LENGTH  200 //1472 is the usual max but can be changed

#define BER_INVERSE  5000        // max 32000 min 1
#define SEND_CONSTANT_DATA 1 //for integration to send constant data
#define IF_BITERROR 1 //enable or disable biterrors
#define SIMULATE_PACKET_DROP 0 // max 100, set to zero to disable
#define VIDEOFRAMES 4 //each of 32bytes

#endif /* CONFIG_H_ */
