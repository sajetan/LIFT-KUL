
#ifndef DEF_COMMUNICATION
#define DEF_COMMUNICATION

#include"main.h"

// below is te original communication.h content
#define MAX_TRANSFER_LENGTH 1472

int init_socket(int tx_port, int rx_port, int timeout_sec);
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

#endif