/*
Ferdinand Hannequart
inverse.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_INVERSE
#define DEF_INVERSE

#include "include/common/globals.h"
#include "include/common/utilities.h"
#include "include/common/add.h"
#include "include/common/sub.h"
#include "include/common/shift.h"

void inverse(WORD z[], WORD a[], WORD m[]);
void inverseTest(WORD print);
void inverseTestHelp(char aChar[], char mChar[], char expChar[], WORD print, WORD* pass);
void subUnsigned(WORD a[], WORD b[], WORD* aIsNeg, WORD bIsNeg);
void subUnsignedTestHelp(char aChar[], char bChar[], char expChar[], WORD aIsNeg, WORD bIsNeg, WORD expIsNeg, WORD* pass, WORD print);
void subUnsignedTest(WORD print);
#endif