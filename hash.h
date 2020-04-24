/*
 * hash.h
 * 
 *
 *  Created on: Mar 23, 2020
 *      Author: Ferdinand Hannequart
 */

#ifndef DEF_HASH
#define DEF_HASH

#include"globals.h"
#include"utilities.h"
#include"sha3.h"

void hash(WORD out[],  void* in, uint16_t sizeHash);
void hash256Test(WORD print);
void hash256TestHelp(char inchar[], char expchar[], WORD* pass, WORD print);

#endif