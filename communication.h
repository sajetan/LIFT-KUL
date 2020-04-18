
#ifndef DEF_COMMUNICATION
#define DEF_COMMUNICATION

#include <stdint.h>
#include <assert.h>
#include<stdint.h>      // for uintx_t
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>

#define MAX_TRANSFER_LENGTH 1472

int init_socket(int tx_port, int rx_port, int timeout_sec);
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

#endif