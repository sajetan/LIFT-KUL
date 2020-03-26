/*
 * hash.h
 * 
 *
 *  Created on: Mar 23, 2020
 *      Author: Ferdinand Hannequart
 */

#ifndef DEF_HASH
#define DEF_HASH

#include"main.h"

void hash256(WORD hash[], WORD input[]);
WORD getNumberBytes(WORD w[]);
void getNumberBytesTest(WORD print);
WORD getNumberZeroBytes(WORD w);
WORD getNumberBytesTestHelp(WORD exp, WORD w[], WORD print);
void hash256Test(WORD print);
#endif