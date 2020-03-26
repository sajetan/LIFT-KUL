/*
 * globals.h
 */

#ifndef INCLUDE_COMMON_GLOBALS_H_
#define INCLUDE_COMMON_GLOBALS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include"add.h"
#include"sub.h"
#include"mod.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef  uint64_t u64;
typedef  uint32_t u32;
typedef  uint16_t u16;
typedef  uint8_t u8;

//change to whatever word size you want to
#define WORD u32
#define DWORD u64

void hex32_decoder(const char *in, size_t len,WORD *out);
void hex16_decoder(const char *in, size_t len,WORD *out);
void hex8_decoder(const char *in, size_t len,WORD *out);
void print_num(WORD *in, WORD size);
void test_convert_hex_array();

#endif /* INCLUDE_COMMON_GLOBALS_H_ */
