/*
Ferdinand Hannequart
inverse.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_INVERSE
#define DEF_INVERSE

#include"main.h"

void inverse(WORD z[], WORD a[], WORD m[]);
void inverseTest(WORD print);
void inverseTestHelp(char aChar[], char mChar[], char expChar[], WORD print, WORD* pass);
void subUnsigned(WORD a[], WORD b[], WORD* aIsNeg, WORD bIsNeg);

#endif