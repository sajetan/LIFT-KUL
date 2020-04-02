/*

 * division.h
 *
 *  Created on: Mar 29, 2020
 *      Author: r0665956
 */

#ifndef DIVISION_H_
#define DIVISION_H_

#include"main.h"

void division(WORD *q, WORD *r, WORD *x, WORD *y);
void shiftBase(WORD *res, WORD a, WORD *y);



//testfunction
void divisionTest(WORD print);
void divisionTestHelp(char xChar[], char yChar[], char expqChar[], char exprChar[], WORD* pass, WORD print);
void shiftBaseTestHelp(WORD a, char yChar[], char expChar[], WORD* pass, WORD print);
void shiftBaseTest(WORD print);


#endif /* DIVISION_H_ */
