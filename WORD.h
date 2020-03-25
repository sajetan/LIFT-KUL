/*
Ferdinand Hannequart
WORD.h

purpose: contains the prototypes, defines and includes
blabla
*/

#ifndef DEF_WORD
#define DEF_WORD

#include"main.h"

WORD equalWord(WORD a[], WORD b[]);
void copyWord(WORD a[], WORD c[]);
void printWord(WORD w[]);
void shiftl1(WORD w[]);
void shiftr1(WORD w[]);
void shiftl1Test( WORD print);
void shiftr1Test( WORD print);
WORD shiftl1TestHelp(WORD test[], WORD exp[], WORD print);
WORD shiftr1TestHelp(WORD test[], WORD exp[], WORD print);

#endif
