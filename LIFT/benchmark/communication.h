/*
 * communication.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef COM_H_
#define COM_H_

#include <stdint.h>
#include "../src/common/utilities.h"
#include "../src/random/random_gen.h"
#include "../src/common/fsm.h"

#define MAX_TRANSFER_LENGTH 1472

#define IFENCRYPT 1 //to enable encryption 1, to disable 0

#define MSGS 5000 // number of messages to send

#define ENCRYPTED_MESSAGE_LENGTH 2 //(32 * ENCRYPTED_MESSAGE_LENGTH) bytes , 32 because 256 bits in random gen


int init_socket(const char* tx_ip,int tx_port, int rx_port);
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

uint16_t make_encryption(uint8_t* data, uint8_t* key, uint32_t seq_num, EntropyPool* pool);
uint16_t make_decryption(uint32_t* seq_num, uint8_t* key, uint8_t* rcv_data);


#endif
