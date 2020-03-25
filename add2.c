/*
 * add2.c

 *
 *  Created on: Mar 23, 2020
 *      Author: r0665956
 */



#include"add2.h"


// Calculates res = (a + b)
// a and b represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements (0-elements not included), b is an array of length_b elements (0-elements not included),
// res has max{length_a,length_b} (+ 1) elements (+1 depending on the last carry) (0-elements not included)
// WORD_length is the amount of bits in WORD (32 if WORD = uint32_t, 16 if WORD = uint32_t)
// size is the fixed amount of elements in the arrays (0-elements included)
void add2(WORD *res, WORD *a, WORD *b, WORD WORD_length, WORD size)
{
	WORD length_a = a[0];
	WORD length_b = b[0];

	WORD c = 0;
	WORD i;
	WORD_2 sum;

	// the addition:
		for (i=1; i<=size-1; i++) {
			sum = (WORD_2) a[i] + (WORD_2) b[i] + (WORD_2) c;
			res[i] = sum;
			c = sum>>WORD_length;
		}

	// setting res[0] right:
		if (length_a >= length_b) {
			if(res[length_a + 1] == 0){
				res[0] = length_a;
			}
			else{
				res[0] = length_a + 1;
			}
		}
		else {
			if (res[length_b + 1] == 0){
				res[0] = length_b;
			}
			else {
				res[0] = length_b + 1;
			}

		}

}





