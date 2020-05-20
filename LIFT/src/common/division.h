/*
 * division.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef DIVISION_H_
#define DIVISION_H_

#include "globals.h"
#include "utilities.h"
#include "mult.h"
#include "add.h"
#include "sub.h"
#include "inverse.h" // to have access to subunsigned

void division(WORD *q, WORD *r, WORD *x, WORD *y);
void shiftBase(WORD *res, WORD a, WORD *y);



//testfunction
void divisionTest(WORD print);
void divisionTestHelp(char xChar[], char yChar[], char expqChar[], char exprChar[], WORD* pass, WORD print);


#endif /* DIVISION_H_ */
