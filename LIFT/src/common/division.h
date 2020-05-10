/*

 * division.h
 *
 *  Created on: Mar 29, 2020
 *      Author: r0665956
 */

#ifndef DIVISION_H_
#define DIVISION_H_

#include "globals.h"
#include "utilities.h"
#include "mult.h"
#include "add.h"
#include "sub.h"

void division(WORD *q, WORD *r, WORD *x, WORD *y);
void shiftBase(WORD *res, WORD a, WORD *y);



//testfunction
void divisionTest(WORD print);
void divisionTestHelp(char xChar[], char yChar[], char expqChar[], char exprChar[], WORD* pass, WORD print);


#endif /* DIVISION_H_ */