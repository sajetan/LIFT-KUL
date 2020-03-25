/*
Ferdinand Hannequart
main.h

purpose: it contains the prototypes, defines and includes
*/

#ifndef DEF_MAIN
#define DEF_MAIN

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

#define TESTOK() printf("OK, all tests passed for %s\n", __func__);
#define TESTFAIL() printf("FAIL :/ - %s\n", __func__);


// Lien's functions
#include"sub.h"
#include"add.h"
#include"mod.h"

// Ferdinand's functions
#include"gcd.h"
#include"begcd.h"
#include"inverse.h"
#include"word.h"

// Tejas's functions
#include"utilities.h"



#endif