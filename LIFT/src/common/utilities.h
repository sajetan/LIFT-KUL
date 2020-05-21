/*
 * utilities.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "globals.h"


#define UINT8_SIZE sizeof(uint8_t)
#define UINT16_SIZE sizeof(uint16_t)
#define UINT32_SIZE sizeof(uint32_t)
#define UINT64_SIZE sizeof(uint64_t)

typedef struct Timer Timer;
struct Timer
{
	struct timeval  tv;
	struct timeval  tv2;
	uint32_t time_in_mill_1;
	uint32_t time_in_mill_2;
};

void convert(WORD *out, const char *in);
void print_num(WORD *in);
void print_num_WORDype(WORD *in, WORD s, WORD type);
void print_num_size(WORD *in, WORD s);
void print_hex(WORD *in); //prints [0x1,0x2,0x3,0x4] as 0x4321
void print_hex_type(WORD *in, WORD type);
void print_hex_8(uint8_t *in);
void print_hex_WORDype(WORD *in,WORD size, WORD type);
void print_array8(uint8_t *in, uint64_t size);
void print_num_type(uint8_t *in,size_t type);
void print_num_type_length(uint8_t *in,size_t len,size_t type);

void rawbyte2word(WORD *out, uint8_t *in, WORD len);
void word2rawbyte(uint8_t *out, WORD *in, WORD len);
WORD equalWord(WORD a[], WORD b[]);
uint8_t equalArray8(uint8_t a[], uint8_t b[], uint16_t len);
void copyWord(WORD copy[], WORD w[]);
void copyArray8(uint8_t copy[], uint8_t a[], uint16_t len);
void copyArrayWithSize(WORD copy[], WORD w[]);
void copyArrayWithoutLength(WORD copy[], WORD w[]);
void copyArrayWithoutSize(WORD copy[], WORD w[], uint16_t len);
void copyBytes(uint8_t copy[], uint8_t w[], uint16_t len);
void convertWithSize(WORD *out, const char *in, WORD size);
//void copyArray(WORD copy[], WORD w[],WORD len);

WORD getNumberBytes(WORD w[]);
void getNumberBytesTest(WORD print);
WORD getNumberZeroBytes(WORD w);
WORD getNumberBytesTestHelp(WORD exp, WORD w[], WORD print);

void printWord(WORD w[]);
WORD greaterThan(WORD a[], WORD b[]);
void convertTest();
void number2array(WORD array[], uint64_t number);
void text2array(WORD array[], char text[]);
void array2text(char text[], WORD array[]);
void print_array(WORD *in, uint64_t size);
void initArray(WORD *in, WORD size);
void initArray8(uint8_t *in, WORD size);

void convertArray16toArray8(uint8_t *out, uint16_t *in);
void convertArray16toArray8withoutLen(uint8_t *out, uint16_t *in);
void convertArray8toArray16(uint16_t *out, uint8_t *in, WORD len);
WORD geq(WORD a[], WORD b[]);
void updateLen(WORD a[]);

void byte2charWithSize(char *out, uint8_t *in, WORD len,WORD type);
void byte2char(char *out, WORD *in, WORD type);
void char2byte(uint8_t *out, const char *in);
//void hex_decoder(const char *in, WORD len,uint32_t *out);
//void test_convert_hex_array();

void startTimer(Timer *timer);
void startTimerMicro(Timer *timer);
uint32_t valueTimer(Timer *timer);
uint32_t valueTimerMicro(Timer *timer);
void printTimer(Timer* timer);

#endif /* SUB_H_ */
