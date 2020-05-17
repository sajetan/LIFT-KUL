/*
 * communication.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef DEF_COMMUNICATION
#define DEF_COMMUNICATION

#include <stdint.h>
#include <assert.h>
#include <stdint.h>      // for uintx_t
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <arpa/inet.h>
#include "../common/globals.h"

#define MAX_TRANSFER_LENGTH 1472


int init_socket(const char *tx_ip, int tx_port, int rx_port, int timeout_sec);
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

#endif
