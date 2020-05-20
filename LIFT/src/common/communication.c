/*
 * communication.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "communication.h"

struct sockaddr_in rx_addr; 
struct sockaddr_in tx_addr; 
int fd_tx;
int fd_rx;

int init_socket(const char *tx_ip, int tx_port, int rx_port) {

	// create tx and rx sockets

	if ((fd_tx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create TX socket\n");
		return 0;
	}

	if ((fd_rx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create RX socket\n");
		return 0;
	}

	//safety check
	if (fd_rx==0||fd_rx==1||fd_rx==2||fd_tx==0||fd_tx==1||fd_tx==2){
		close_sockets();
		init_socket(tx_ip, tx_port,rx_port);
	}

	// bind tx socket to all valid addresses, and any port

	memset((char *)&rx_addr, 0, sizeof(rx_addr));
	rx_addr.sin_family = AF_INET;
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	rx_addr.sin_port = htons(0);

	if (bind(fd_tx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("bind tx failed");
		return 0;
	}

	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 10;

	setsockopt(fd_rx, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));

	// bind rx socket to all valid addresses, and a specific port

	rx_addr.sin_port = htons(rx_port);

	if (bind(fd_rx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("bind rx failed");
		return 0;
	}

	// now define tx_addr, the address to whom we want to send messages

	memset((char *) &tx_addr, 0, sizeof(tx_addr));
	tx_addr.sin_family = AF_INET;
	tx_addr.sin_port = htons(tx_port);

	if (inet_aton(tx_ip, &tx_addr.sin_addr)==0) {
		printf("inet_aton() failed\n");
		return 0;
	}

	return 1;
}

int close_sockets() {
	close(fd_tx);
	close(fd_rx);
	return RETURN_SUCCESS;
}

int send_message(uint8_t* data, int length) {
	if (sendto(
			fd_tx,
			data,
			length,
			0,
			(struct sockaddr *)&tx_addr,
			sizeof(tx_addr))==-1)
		return 0;
	return 1;
}

int receive_message(uint8_t* data) {

	socklen_t addrlen = sizeof(rx_addr);

	return recvfrom(
			fd_rx,
			data,
			MAX_TRANSFER_LENGTH,
			0,
			(struct sockaddr *)&rx_addr,
			&addrlen);
}
