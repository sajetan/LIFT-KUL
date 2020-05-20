/*
 * mod.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef MOD_H_
#define MOD_H_

#include "globals.h"
#include "utilities.h"
#include "division.h"
#include "add.h"
#include "sub.h"

void mod(WORD *res, WORD *a, WORD *n);
void smallMod(WORD *res, WORD *a, WORD *n);
void mod_sub(WORD *res, WORD *a, WORD *b, WORD *n);
//testfunctions
void modTest(WORD print);
void modTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print);


#endif /* MOD_H_ */
