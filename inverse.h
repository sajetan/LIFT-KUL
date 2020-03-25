/*
Ferdinand Hannequart
inverse.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_INVERSE
#define DEF_INVERSE

#include"main.h"

void inverse(WORD* z, WORD a, WORD m);
void inverseTest();
void inverseTestHelp(WORD a, WORD m, WORD zExp);
WORD subtractUnsigned(WORD a, WORD b, WORD* aIsNeg, WORD bIsNeg);

#endif