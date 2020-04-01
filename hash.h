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

void hash256(WORD hash[],  void* input);
WORD getNumberBytes(WORD w[]);
void getNumberBytesTest(WORD print);
WORD getNumberZeroBytes(WORD w);
WORD getNumberBytesTestHelp(WORD exp, WORD w[], WORD print);
void hash256Test(WORD print);
void hash256TestHelp(char inchar[], char expchar[], WORD* pass, WORD print);

#endif