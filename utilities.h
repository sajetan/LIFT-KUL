/*
 * utilities.h
 *
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include"main.h"

void hex_decoder(const char *in, size_t len,uint32_t *out);
void print_num(uint32_t *in, uint32_t size);
void test_convert_hex_array();


#endif /* SUB_H_ */
