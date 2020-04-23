/*
Chacha20 implementation originally djb and simplified code version by insanecoding. Adapted to use for drone project by sajetan

Copyright (C) 2014 insane coder (http://insanecoding.blogspot.com/, http://chacha20.insanecoding.org/)

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef CHACHA20_H_
#define CHACHA20_H_

#include <stdint.h>
#include <stddef.h>
#include "main.h"

#define ROTL32(v, n) ((v) << (n)) | ((v) >> (32 - (n)))

#define LE(p) (((uint32_t)((p)[0])) | ((uint32_t)((p)[1]) << 8) | ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24))
#define FROMLE(b, i) (b)[0] = i & 0xFF; (b)[1] = (i >> 8) & 0xFF; (b)[2] = (i >> 16) & 0xFF; (b)[3] = (i >> 24) & 0xFF;
#define LIFT_LE(p) (((uint32_t)((p)[0*BYTE]) << 24) | ((uint32_t)((p)[1*BYTE]) << 16) | ((uint32_t)((p)[2*BYTE]) << 8) | ((uint32_t)((p)[3*BYTE])))

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  uint32_t schedule[16];
  uint32_t keystream[16];
  size_t available;
} chacha20_ctx;

//Call this to initilize a chacha20_ctx, must be called before all other functions
void chacha20_init_setup(chacha20_ctx *ctx, const uint8_t *key, size_t length, uint8_t nonce[8]);

//Call this if you need to process a particular block number
void chacha20_counter_set(chacha20_ctx *ctx, uint64_t counter);

//Raw keystream for the current block, convert output to uint8_t[] for individual bytes. Counter is incremented upon use
void chacha20_block(chacha20_ctx *ctx, uint32_t output[16]);

//Encrypt an arbitrary amount of plaintext, call continuously as needed
void chacha20_encrypt(chacha20_ctx *ctx, const uint8_t *in, uint8_t *out, size_t length);

//Decrypt an arbitrary amount of ciphertext. Actually, for chacha20, decryption is the same function as encryption
void chacha20_decrypt(chacha20_ctx *ctx, const uint8_t *in, uint8_t *out, size_t length);

void chacha_init_ctx(chacha20_ctx *ctx);

#ifdef __cplusplus
}
#endif


#endif /* CHACHA20_H_ */