///*
//
// * sub.c
// *
// *  Created on: Mar 17, 2020
// *      Author: r0665956
// */

#include"sub.h"

// Calculates res = (a - b).
// a and b represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements, b is an array of length_b elements, res has length_a elements
// with a>= b !!! (we will never use negative numbers in our implementation)
void sub(WORD *res, WORD *a, WORD *b)
{
		WORD length_a = a[0];
		WORD length_b = b[0];
		WORD c = 0;
		WORD i;
		res[0] = length_a;
		for (i=1; i<=length_b; i++) {
			res[i] = a[i] - b[i] - c;
			if (a[i] < b[i] + c) {
				c = 1;
		     	}
			else {
				c = 0;
			   }
		 }
		for (i=length_b+1; i<=length_a; i++) {
			res[i] = a[i]- c;
			if (a[i] <  c) {
				c = 1;
		     	}
			else {
				c = 0;
			   }
		 }
}
