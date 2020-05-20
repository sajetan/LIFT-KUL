/*
 * shift.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef DEF_SHIFT
#define DEF_SHIFT

#include "globals.h"
#include "utilities.h"

void shiftl1(WORD w[]);
void shiftr1(WORD w[]);
void shiftl1Test( WORD print);
void shiftr1Test( WORD print);
WORD shiftl1TestHelp(WORD test[], WORD exp[], WORD print);
WORD shiftr1TestHelp(WORD test[], WORD exp[], WORD print);

#endif
