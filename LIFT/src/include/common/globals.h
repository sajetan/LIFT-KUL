/*
 * globals.h
 */

#ifndef INCLUDE_COMMON_GLOBALS_H_
#define INCLUDE_COMMON_GLOBALS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

enum LIFT_DRONE_SESSION_STATES {
	DRONE_SESSION_EST_INIT = 0,
	DRONE_SESSION_EST_WAIT_RESPONSE,
	DRONE_SESSION_EST_VERIFY_RESPONSE,
	DRONE_SESSION_EST_WAIT_COMMUNICATION,
	DRONE_SESSION_EST_TERMINATE,
} ;

enum LIFT_BST_SESSION_STATES {
	BST_SESSION_EST_INIT = 0,
	BST_SESSION_EST_WAIT_RESPONSE,
	BST_SESSION_EST_VERIFY_RESPONSE,
	BST_SESSION_EST_WAIT_COMMUNICATION,
	BST_SESSION_EST_TERMINATE,
} ;


void hex_decoder(const char *in, size_t len,uint32_t *out);
void print_num(uint32_t *in, uint32_t size);
void test_convert_hex_array();

#endif /* INCLUDE_COMMON_GLOBALS_H_ */
