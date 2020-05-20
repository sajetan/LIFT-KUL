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

#define PRINT_STATE()   if(1)\ 						// set this to 1 if you want to see the current state
		printf("[ Current STATE:  %s ]\n", __func__);
#define DEBUG_FSM(s)    if(0)\						// set this to 1 if you want to see additional info about the FSM (useful for debugging)
		printf("\t%s \n",s);
#define PRINT_VIDEO_FRAME(s,l)    if(1)\
		printf("\tVideo Seq num [%d] len [%d] \n",s,l);

#define DEBUG_SIGNATURE 0							// set this to 1 if you want to print out all values related to the STS protocol
#define PRINT_CONTENT_UDP_PACKET 0					// content packets
#define PRINT_BITFLIP() if(1)\						// Interesting! set this to 1 if you want to get notified when CRC detects a bit flip.
						printf("CRC Incorrect, Packet will be dropped --------------------\n"); 


#define MAX_TRANSFER_LENGTH  200 //1472 is the usual max but can be changed
#define SEND_CONSTANT_DATA 1 //for integration to send constant data
#define VIDEOFRAMES 4 //each of 32bytes
#define DELAY_VIDEO 0 // in µs, sets the delay between video frames in case the communication cannot follow

#define IF_BITERROR 1 			//enable or disable bit errors
#define BER_INVERSE  4000       // max 32000 min 1, represents the inverse of the desired BER
#define SIMULATE_PACKET_DROP 0 	// max 100, set to zero to disable

#endif /* CONFIG_H_ */
