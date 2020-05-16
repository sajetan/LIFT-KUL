#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include "communication.h"


#define IFENCRYPT 1
struct sockaddr_in rx_addr; 
struct sockaddr_in tx_addr; 
int fd_tx;
int fd_rx;

int init_socket(int tx_port, int rx_port, int timeout_sec) {
    
    char *dst_ip = "127.0.0.1";

	// create tx and rx sockets

	if ((fd_tx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create TX socket\n");
        return 0;
    }

    if ((fd_rx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create RX socket\n");
        return 0;
    }

    // bind tx socket to all valid addresses, and any port

    memset((char *)&rx_addr, 0, sizeof(rx_addr));
	rx_addr.sin_family = AF_INET;
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	rx_addr.sin_port = htons(0);

	if (bind(fd_tx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("bind failed");
		return 0;
	}
    
    struct timeval read_timeout;
    read_timeout.tv_sec = 10;
    read_timeout.tv_usec = 0;

    setsockopt(fd_rx, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
    
    // bind rx socket to all valid addresses, and a specific port 
 
    rx_addr.sin_port = htons(rx_port);

	if (bind(fd_rx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("bind failed");
		return 0;
	}

    // now define tx_addr, the address to whom we want to send messages
	
    memset((char *) &tx_addr, 0, sizeof(tx_addr));
	tx_addr.sin_family = AF_INET;
	tx_addr.sin_port = htons(tx_port);

	if (inet_aton(dst_ip, &tx_addr.sin_addr)==0) {
		printf("inet_aton() failed\n");
		return 0;
	}

    return 1;
}

int close_sockets() {
    close(fd_tx);
    close(fd_rx);
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



uint16_t make_encryption(uint8_t* data, uint8_t* key, uint32_t seq_num, EntropyPool* pool){

    uint8_t mac_tag[MAC_TAG_LENGTH] = {0};    
    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
    WORD len=0;
    WORD i=0;
	initArray8(data, MAX_DATA_LENGTH);

    uint8_t videoframe[VIDEOFRAME_LENGTH] = {0};
    uint8_t data_payload[MAX_DATA_LENGTH] = {0};
    uint8_t plaintext[MAX_DATA_LENGTH] = {0};	
    uint16_t plaintext_len =0;

    WORD n=0;
    WORD rand[256] = {0};

    for(i = 0; i<SEQ_LENGTH; i++){
    	seq_num >>= 8*i;
    	plaintext[i] = seq_num;
    }
    i=SEQ_LENGTH;
    for (n=0;n<ENCRYPTED_MESSAGE_LENGTH;n++){

		random_gen(rand, 256, pool);

		word2rawbyte(&plaintext[i], rand, (32)); 

		i+=32;
    }
	plaintext_len=i;
    random_gen(rand, CHACHA_NONCE_LENGTH*8, pool);
    word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);


	aead_chacha20_poly1305(mac_tag,ciphertext, key, CHACHA_KEY_LENGTH, nonce, plaintext, plaintext_len, "50515253c0c1c2c3c4c5c6c7"); 

	//sending plaintext length in the beginning
	data[0]=plaintext_len;
	data[1]=plaintext_len>>8;
    len += 2;

    for(i=0;i<CHACHA_NONCE_LENGTH;i++){
    	data[len + i] = nonce[i];
    }
    len += CHACHA_NONCE_LENGTH;

    for(i=0;i<plaintext_len;i++){
    	data[len + i] = ciphertext[i];
    }
    len +=plaintext_len;

    for(i=0;i<MAC_TAG_LENGTH;i++){
    	data[len + i] = mac_tag[i];
    }
    len += MAC_TAG_LENGTH;

    assert(len = CHACHA_NONCE_LENGTH + plaintext_len + MAC_TAG_LENGTH+2); // check
    return len;
}

uint16_t make_decryption(uint32_t *seq_num, uint8_t* key, uint8_t* rcv_data){
	uint16_t ciphertext_len=0;
    uint8_t ciphertext[MAX_DATA_LENGTH]={0};
    uint8_t plaintext[MAX_DATA_LENGTH]={0};
    uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};

	WORD i = 0;
    uint8_t valid = 0;
    uint16_t start = 0;
    uint16_t len = 0;
    ciphertext_len = (rcv_data[1]<<8|rcv_data[0]);
    start += 2;
    for(i = 0; i<CHACHA_NONCE_LENGTH; i++){
        nonce[i] = rcv_data[start + i];
    }
    start += CHACHA_NONCE_LENGTH;
    for(i = 0; i<ciphertext_len; i++){
        ciphertext[i] = rcv_data[start + i];
    }
    start += ciphertext_len;

    for(i = 0; i<MAC_TAG_LENGTH; i++){
        mac_tag[i] = rcv_data[start + i];
    }
    start += MAC_TAG_LENGTH;
    len = start;

   // mac check
  valid = verify_mac_aead_chacha20_poly1305(mac_tag, key, CHACHA_KEY_LENGTH, nonce, ciphertext, ciphertext_len, "50515253c0c1c2c3c4c5c6c7");
    //decrypt
    decrypt_init(plaintext, key, CHACHA_KEY_LENGTH, nonce, ciphertext, ciphertext_len, 0);

    *seq_num = (plaintext[3]<<24|plaintext[2]<<16|plaintext[1]<<8|plaintext[0]);
	
    return RETURN_SUCCESS;
}




