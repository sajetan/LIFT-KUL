/*
 *Ferdinand Hannequart
 *inverse.h
 */

#ifndef DEF_INVERSE
#define DEF_INVERSE

#include"main.h"

void inverse(WORD* z, WORD a, WORD m);
void inverseTest(WORD print);
void inverseTestHelp(WORD a, WORD m, WORD zExp, WORD print, WORD* pass);
WORD subtractUnsigned(WORD a, WORD b, WORD* aIsNeg, WORD bIsNeg);

#endif
