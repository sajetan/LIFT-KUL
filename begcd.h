/*
Ferdinand Hannequart
begcd.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_BEGCD
#define DEF_BEGCD

#include"main.h"

void begcd(WORD* a, WORD* b, WORD* r, WORD x, WORD y);
void begcdTest();
void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp);
void verifyCoefficients(WORD x, WORD y, WORD expGCD );
#endif