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


void sig_gen_pre(WORD *r, WORD *k, WORD *n, WORD *G_x, WORD *G_y);
void sig_gen(WORD *s, WORD *m, WORD *k, WORD *n, WORD *d, WORD *r);
WORD sig_ver(WORD *r, WORD *s, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y);

//testfunctions

void signatureTest();
void signatureTestHelp(char mChar[], char nChar[], char dChar[], char G_xChar[], char G_yChar[], char Q_xChar[], char Q_yChar[], WORD expected);

#endif /* SIGNATURE_H_ */
