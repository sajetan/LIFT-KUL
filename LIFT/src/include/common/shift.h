/*
Ferdinand Hannequart
WORD.h

purpose: contains the prototypes, defines and includes
*/

#ifndef DEF_SHIFT
#define DEF_SHIFT

#include "include/common/globals.h"
#include "include/common/utilities.h"

void shiftl1(WORD w[]);
void shiftr1(WORD w[]);
void shiftl1Test( WORD print);
void shiftr1Test( WORD print);
WORD shiftl1TestHelp(WORD test[], WORD exp[], WORD print);
WORD shiftr1TestHelp(WORD test[], WORD exp[], WORD print);

#endif
