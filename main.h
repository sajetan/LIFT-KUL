/*
main.h

purpose: it contains the prototypes, defines and includes
*/

#ifndef MAIN_H_
#define MAIN_H_

#include<time.h>        // for timing
#include<stdio.h>       // for prints
#include<stdint.h>      // for uint32_t

#include<stddef.h>
#include<string.h>
#include<ctype.h>       // for toupper()


#define WORD uint32_t
#define WORD_2 uint64_t // should be removed
#define BIT ((WORD) (sizeof(WORD)*8) )
#define SIZE ((WORD) 40 )
<<<<<<< HEAD
#define TEST(pass)          if(pass)\
                            printf("OK, all tests passed for %s\n", __func__);\
                            else \
                            printf("FAIL :/ - %s\n", __func__);
#define BEGINTEST(print)    if (print)\
                            printf("\n------- begin %s -------\n", __func__);
#define ENDTEST(print)      if (print)\
                            printf("------- end %s -------\n", __func__);

=======
>>>>>>> upstream/functions

// Lien's functions
#include"sub.h"
#include"add.h"
#include"mod.h"

// Ferdinand's functions
#include"gcd.h"
#include"inverse.h"
#include"shift.h"
<<<<<<< HEAD
#include"sha3.h"
#include"hash.h"
=======
>>>>>>> upstream/functions

// Tejas's functions
#include"utilities.h"



<<<<<<< HEAD
#endif
=======
#endif
>>>>>>> upstream/functions
