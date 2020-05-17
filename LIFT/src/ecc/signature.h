/*
 * signature.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef SIGNATURE_H_
#define SIGNATURE_H_

#include "../common/main.h"
#include "../common/globals.h"
#include "../common/utilities.h"
#include "p256.h"
#include "../common/mod.h"
#include "../common/mult.h"
#include "../common/inverse.h"
#include "../common/division.h"
#include "../random/random_gen.h"
#include "../hash/hash.h"

void signature_gen(WORD *output, WORD *key, WORD *message, WORD *n, WORD *G_x, WORD *G_y);
LIFT_RESULT sig_ver(WORD *input, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y);

//testfunctions

void signatureTest();
void signatureTest2();
void signatureTestHelp(char mChar[], char nChar[], char dChar[], char G_xChar[], char G_yChar[], char Q_xChar[], char Q_yChar[], WORD expected);

#endif /* SIGNATURE_H_ */
