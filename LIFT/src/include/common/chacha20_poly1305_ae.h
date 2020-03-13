/*
 * chacha20_poly1305_ae.h
 */

#ifndef COMMON_CHACHA20_POLY1305_AE_H_
#define COMMON_CHACHA20_POLY1305_AE_H_

#include "globals.h"

uint32_t chacha20_encrypt(uint32_t a);
uint32_t chacha20_decrypt(uint32_t a);
uint32_t chacha20_initial_setup(uint32_t a);
uint32_t chacha20_block_operation(uint32_t a);


#endif /* COMMON_CHACHA20_POLY1305_AE_H_ */
