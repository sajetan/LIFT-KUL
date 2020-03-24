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

#define WORD uint32_t
#define WORD_2 uint64_t

void hex_decoder(const char *in, size_t len,uint32_t *out);
void print_num(uint32_t *in, uint32_t size);
void test_convert_hex_array();

#endif /* INCLUDE_COMMON_GLOBALS_H_ */
