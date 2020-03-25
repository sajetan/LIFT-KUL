/*
Ferdinand Hannequart
gcd.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_GCD
#define DEF_GCD

#include"main.h"

#define GCDTESTPRINT(u, v, exp) if(exp == gcd(u, v)) \
                                printf("Test OK: The gcd of %d and %d is %d\n", u, v, exp);\
                                else \
                                printf("FAIL: The gcd of %d and %d is %d, expected %d\n", u, v, gcd(u, v), exp);


WORD gcd(WORD u, WORD v);
void gcdTest();

#endif