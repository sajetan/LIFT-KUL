/*
 * chacha20_poly1305_interface.h
 * sajetan
 */

#ifndef CHACHA20_POLY1305_INTERFACE_H_
#define CHACHA20_POLY1305_INTERFACE_H_

//#include"main.h"
#include "chacha20.h"

#define MAX_MSG_SIZE 2048 //in bytes
#define CHACHA_KEY_LENGTH 32 //in bytes
#define CHACHA_NONCE_LENGTH 8 //in bytes
#define MAC_TAG_LENGTH 16 //in bytes

#define POLY_PADDING_LENGTH 16 //in bytes


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct poly1305_msg_struct {
		unsigned char aad[MAX_MSG_SIZE];
		unsigned char msg[MAX_MSG_SIZE];
        unsigned char pad1[32];
        unsigned char pad2[32];
        uint64_t msg_len;
        uint64_t aad_len;
        uint64_t total_len;
} poly1305_msg_struct;

void poly1305_msg_construction(u8 *poly_msg,poly1305_msg_struct *in);

void encrypt_init(u8 *ciphertext, u8 *key_in, u8 key_length, u8 *nonce_in, u8 *plain_text_in, u8 len, u64 counter);

void print_num_type(uint8_t *in,size_t type);

void poly1305_key_gen(u8 *poly_key, u8 *key,u8 *nonce);

void aead_chacha20_poly1305(uint8_t *output,uint8_t *ciphertext_out,uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len, const char *aad);
uint16_t verify_mac_aead_chacha20_poly1305(uint8_t *rcv_mac_tag, uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len, const char *aad);
void paddingCheck(poly1305_msg_struct  *in, uint8_t padding_length);

//test functions
void padding_test();
void aead_test();
void test_ec();


#endif /* CHACHA20_POLY1305_INTERFACE_H_ */
