/*
 * chacha20_poly1305_interface.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef CHACHA20_POLY1305_INTERFACE_H_
#define CHACHA20_POLY1305_INTERFACE_H_

#include "chacha20.h"

#define decrypt_init encrypt_init //encryption and decryption uses the same code
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
	uint8_t aad[MAX_MSG_SIZE];
	uint8_t msg[MAX_MSG_SIZE];
	uint8_t pad1[32];
	uint8_t pad2[32];
	uint64_t msg_len;
	uint64_t aad_len;
	uint64_t total_len;
} poly1305_msg_struct;

void poly1305_msg_construction(u8 *poly_msg,poly1305_msg_struct *in);

void encrypt_init(u8 *ciphertext, u8 *key_in, u8 key_length, u8 *nonce_in, u8 *plain_text_in, u32 len, u64 counter);

void poly1305_key_gen(u8 *poly_key, u8 *key,u8 *nonce);

void aead_chacha20_poly1305(uint8_t *output,uint8_t *ciphertext_out,uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len);
uint16_t verify_mac_aead_chacha20_poly1305(uint8_t *rcv_mac_tag, uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len);
void paddingCheck(poly1305_msg_struct  *in, uint16_t padding_length);

//test functions
void padding_test();
void aead_test();
void test_ec();


#endif /* CHACHA20_POLY1305_INTERFACE_H_ */
