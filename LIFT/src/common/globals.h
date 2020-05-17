/*
 * globals.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <time.h>        // for timing
#include <sys/time.h>    // for timing
#include <stdio.h>       // for prints
#include <stdint.h>      // for uintx_t
#include <pthread.h>     // threads
#include <unistd.h>      // for sleep()
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
//#inclusde <stdlib.h>
//#include <stdio.h>
//#include <string.h>


#define FALSE 0
#define TRUE 1


//new
#define P(print)    printf("hello %x \n", print);

#define WORD uint16_t

#define SIZEHASH 256

#define NUMBEROFBITS 258    // max number of bits our arrays will have to hold
#define WORD_2 uint32_t
#define WORD_3 uint64_t
#define BYTE sizeof(WORD)
#define BIT (BYTE*8)
//#define SIZE ((NUMBEROFBITS- (NUMBEROFBITS % BIT))/BIT +1 + ((NUMBEROFBITS % BIT) != 0))
#define SIZE 66
//#define SIZEPOINT 33

//#define POINT_LEN 32 // length of a point coordonate in bytes
#define SIG_LEN 64 // max length of the signature text
#define CIPHER_LEN 32 // max length of the cipher text
#define SIZE_EC_KEY 256 // size of the EC private key (in bits!)
#define SIZE_SESSION_KEY 256 // size of the EC private key (in bits!)

//maybe in future i'll use this and remove all the array size issues
typedef struct{
	uint64_t len;
	uint8_t word[SIZE*8];
} bigint8;

typedef struct{
	uint64_t len;
	uint16_t word[SIZE];
} bigint16;

typedef struct{
	uint64_t len;
	uint32_t word[SIZE];
} bigint32;

typedef struct{
	uint64_t len;
	uint64_t word[SIZE];
} bigint64;


typedef enum{
	RETURN_INVALID = 0,
	RETURN_SUCCESS
} LIFT_RESULT;



#define TEST(pass)          if(pass)\
		printf("OK, all tests passed for %s\n", __func__);\
		else \
		printf("FAIL :/ - %s\n", __func__);
#define BEGINTEST(print)    if (print)\
		printf("\n------- begin %s -------\n", __func__);
#define ENDTEST(print)      if (print)\
		printf("------- end %s -------\n\n", __func__);


#endif
