/*
 * sub.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef SUB_H_
#define SUB_H_

#include "globals.h"
#include "utilities.h"

void sub(WORD *res, WORD *a, WORD *b);
void subSelf( WORD *a, WORD *b);
//test functions
void subTest(WORD print);
void subTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);
void subSelfTest(WORD print);
void subSelfTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);

#endif /* SUB_H_ */
