/*
 * hash.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef DEF_HASH
#define DEF_HASH

#include "../common/globals.h"
#include "../common/utilities.h"
#include "sha3.h"

void hash(WORD out[],  WORD* in, uint16_t sizeHash);
void hash8(uint8_t out[],  uint8_t in[], uint16_t numberBytesIn, uint16_t numberBytesOut, uint16_t sizeHash);
void hash256Test(WORD print);
void hash256TestHelp(char inchar[], char expchar[], WORD* pass, WORD print);

#endif
