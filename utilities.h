/*
 * utilities.h
 *
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include"main.h"

void convert(WORD *out, const char *in);
void print_num(WORD *in);
void print_hex(uint32_t *in); //prints [0x1,0x2,0x3,0x4] as 0x4321
WORD equalWord(WORD a[], WORD b[]);
void copyWord(WORD copy[], WORD w[]);
void copyArrayWithSize(WORD copy[], WORD w[]);
void convertWithSize(WORD *out, const char *in, size_t size);
void printWord(WORD w[]);
WORD greaterThan(WORD a[], WORD b[]);
void convertTest();
void number2array(WORD array[], uint64_t number);
void text2array(WORD array[], char text[]);
void array2text(char text[], WORD array[]);
void print_array(WORD *in, uint64_t size);
//void hex_decoder(const char *in, size_t len,uint32_t *out);
//void test_convert_hex_array();


#endif /* SUB_H_ */
