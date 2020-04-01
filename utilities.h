/*
 * utilities.h
 *
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include"main.h"

void convert(WORD *out, const char *in);
void print_num(WORD *in);
WORD equalWord(WORD a[], WORD b[]);
void copyWord(WORD copy[], WORD w[]);
void printWord(WORD w[]);
WORD greaterThan(WORD a[], WORD b[]);
void convertTest();
void number2array(WORD array[], uint64_t number);
void text2array(WORD array[], char text[]);
void array2text(char text[], WORD array[]);
//void hex_decoder(const char *in, size_t len,uint32_t *out);
//void test_convert_hex_array();

#endif /* SUB_H_ */
