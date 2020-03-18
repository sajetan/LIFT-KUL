/*
 * add.c

 *
 *  Created on: Mar 17, 2020
 *      Author: r0665956
 */

//WORD = uint32_t;


#include"add.h"


// Calculates res = (a + b)
// a and b represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements, b is an array of length_b elements, res has (max{length_a,length_b} + 1) elements
void add(WORD *res, WORD *a, WORD *b)
{
	WORD length_a = a[0];
	WORD length_b = b[0];

	WORD c = 0;
	WORD i;
	WORD_2 sum;

	if (length_a >= length_b) {
		res[0] = length_a + 1;
		for (i=1; i<=length_b; i++) {
			sum = (WORD_2) a[i] + (WORD_2) b[i] + (WORD_2) c;
			res[i] = sum;
			c = sum>>32;  //c = sum>>WORD_length
		}
		for (i=length_b+1; i<=length_a; i++) {
			sum = (WORD_2) a[i] + (WORD_2) c;
			res[i] = sum;
			c = sum>>32;  //c = sum>>WORD_length
		}
		res[length_a+1] = c;

	}

	if (length_a < length_b) {
			res[0] = length_b + 1;
			for (i=1; i<=length_a; i++) {
				sum = (WORD_2) a[i] + (WORD_2) b[i] + (WORD_2) c;
				res[i] = sum;
				c = sum>>32;  //c = sum>>WORD_length
			}
			for (i=length_a+1; i<=length_b; i++) {
				sum = (WORD_2) b[i] + (WORD_2) c;
				res[i] = sum;
				c = sum>>32;  //c = sum>>WORD_length
			}
			res[length_b+1] = c;

		}
}





