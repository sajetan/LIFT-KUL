
 /* mult.h
 *
 *  Created on: Mar 28, 2020
 *      Author: r0665956
 */

#ifndef MULT_H_
#define MULT_H_

#include"globals.h"
#include"utilities.h"

void mult(WORD *res, WORD *a, WORD *b);
void smallMult(WORD *res, WORD a, WORD *b);

//testfunctions

void multTest(WORD print);
void multTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);
void smallMultTest(WORD print);
void smallMultTestHelp(WORD a, char bChar[], char expChar[], WORD* pass, WORD print);

#endif /* MULT_H_ */
