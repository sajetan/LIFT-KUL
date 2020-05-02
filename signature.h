/*
 * signature.h
 *
 *  Created on: Mar 30, 2020
 *      Author: r0665956
 */

#ifndef SIGNATURE_H_
#define SIGNATURE_H_

#include"main.h"
#include"globals.h"
#include"utilities.h"
#include"p256.h"
#include"mod.h"
#include"mult.h"
#include"inverse.h"
#include"division.h"
#include"random.h"
#include"hash.h"

void signature_gen(WORD *output, WORD *key, WORD *message, WORD *n, WORD *G_x, WORD *G_y);
WORD sig_ver(WORD *input, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y);

//testfunctions

void signatureTest();
void signatureTest2();
void signatureTestHelp(char mChar[], char nChar[], char dChar[], char G_xChar[], char G_yChar[], char Q_xChar[], char Q_yChar[], WORD expected);

#endif /* SIGNATURE_H_ */
