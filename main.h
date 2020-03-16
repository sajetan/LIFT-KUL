/*
Ferdinand Hannequart
main.h

purpose: it contains the prototypes, defines and includes
*/

#ifndef DEF_MAIN
#define DEF_MAIN

#define WORD uint32_t
#define BIT ((WORD) (sizeof(WORD)*8) )
#define SIZE ((WORD) 40 )

#include"gcd.h"
#include"begcd.h"
#include"inverse.h"
#include"WORD.h"

// below: includes and defines for test functions

#define TESTOK() printf("OK, all tests passed for %s\n", __func__);
#define TESTFAIL() printf("FAIL :/ - %s\n", __func__);
#define COUCOU() printf("Coucou");

#include<time.h>        // for timing
#include<stdio.h>       // for prints
#include<stdint.h>      // for uint32_t



#endif