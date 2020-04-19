/*
 * globals.h
 *
 *  Created on: April 19
 *      Author: Ferdinand Hannequart
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include<time.h>        // for timing
#include<sys/time.h>    // for timing
#include<stdio.h>       // for prints
#include<stdint.h>      // for uintx_t
#include<pthread.h>     // threads
#include<unistd.h>      // for sleep()
//#include <stdlib.h>
#include<stddef.h>
#include<string.h>
#include <assert.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>


#define FALSE 0
#define TRUE 1


//new
#define P(print)    printf("hello %d \n", print);

#define WORD uint16_t

#define SIZEHASH 256

#define NUMBEROFBITS 258    // max number of bits our arrays will have to hold
#define WORD_2 uint32_t
#define WORD_3 uint64_t
#define BYTE sizeof(WORD)
#define BIT (BYTE*8)
//#define SIZE ((NUMBEROFBITS- (NUMBEROFBITS % BIT))/BIT +1 + ((NUMBEROFBITS % BIT) != 0))
#define SIZE 200



#define TEST(pass)          if(pass)\
                            printf("OK, all tests passed for %s\n", __func__);\
                            else \
                            printf("FAIL :/ - %s\n", __func__);
#define BEGINTEST(print)    if (print)\
                            printf("\n------- begin %s -------\n", __func__);
#define ENDTEST(print)      if (print)\
                            printf("------- end %s -------\n\n", __func__);


#endif
