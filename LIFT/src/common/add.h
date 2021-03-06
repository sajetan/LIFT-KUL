/*
 * add.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef ADD_H_
#define ADD_H_

#include "globals.h"
#include "utilities.h"
#include "sub.h"

void add(WORD *res, WORD *a, WORD *b);
void addSelf(WORD *a, WORD *b);
//test functions
void addTest(WORD print);
void addTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);
void addSelfTest(WORD print);
void addSelfTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);
void addAndSub(WORD *res, WORD *a, WORD *b, WORD subtraction);

#endif /* ADD2_H_ */
