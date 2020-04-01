/*
main.h

purpose: it contains the prototypes, defines and includes
*/

#ifndef MAIN_H_
#define MAIN_H_

#include<time.h>        // for timing
#include<sys/time.h>        // for timing
#include<stdio.h>       // for prints
#include<stdint.h>      // for uint32_t

#include<stddef.h>
#include<string.h>
#include<ctype.h>       // for toupper()


#define WORD uint16_t
#define WORD_2 uint64_t
#define BYTE sizeof(WORD)
#define BIT (BYTE*8)
#define SIZE ((WORD) 50 )

#define WORDR uint16_t // for entropy pool, must be higher than 8 bit
#define BYTER sizeof(WORDR)
#define BITR (sizeof(WORDR)*8)
#define SIZER 50 // 800 bits, should be large enough for personalization string, hash values, image or counter


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

// Ferdinand's functions
#include"inverse.h"
#include"shift.h"
#include"sha3.h"
#include"hash.h"
#include"random.h"


// Tejas's functions
#include"utilities.h"



#endif
