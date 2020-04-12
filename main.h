/*
main.h

purpose: it contains the prototypes, defines and includes
*/

#ifndef MAIN_H_
#define MAIN_H_

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

#define P(print)    printf("hello %d \n", print);

#define WORD uint16_t

#define SIZEHASH 256

#define NUMBEROFBITS 258    // max number of bits our arrays will have to hold
#define WORD_2 uint32_t
#define WORD_3 uint64_t
#define BYTE sizeof(WORD)
#define BIT (BYTE*8)
//#define SIZE ((NUMBEROFBITS- (NUMBEROFBITS % BIT))/BIT +2)
#define SIZE 100



#define TEST(pass)          if(pass)\
                            printf("OK, all tests passed for %s\n", __func__);\
                            else \
                            printf("FAIL :/ - %s\n", __func__);
#define BEGINTEST(print)    if (print)\
                            printf("\n------- begin %s -------\n", __func__);
#define ENDTEST(print)      if (print)\
                            printf("------- end %s -------\n\n", __func__);


// Lien's functions
#include"add.h"
#include"sub.h"
#include"mod.h"
#include"mult.h"
#include"division.h"

// Ferdinand's functions
#include"inverse.h"
#include"shift.h"
#include"sha3.h"
#include"hash.h"
#include"random.h"


// Tejas's functions
#include"utilities.h"


#endif
