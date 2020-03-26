/*
 * mont_mult.c

 *
 *  Created on: Mar 24, 2020
 *      Author: r0665956
 */

#include"mont_mult.h"


// Calculates res = a * b * r^(-1) mod n.
// a, b, n, n_prime represent operands of size elements
// res has (size+1) elements
void montMul(WORD *res, WORD *a, WORD *b, WORD *n, WORD *n_prime, WORD size)
{
// multiplication
WORD i;
WORD c;
WORD j;
WORD_2 sum;
WORD t[65] = {0};

for (i = 0; i<size; i++){
	c = 0;
	for (j = 0; j<size; j++){
		sum = (WORD_2) t[i + j] + (WORD_2) a[j] * (WORD_2) b[i] + (WORD_2) c;
		t[i + j] = sum;
		c = sum >> 32;
	}
	t[i + size] = c;
}


// reduction

WORD z;
WORD k;

for (i = 0; i<size; i++){
	c = 0;
	z = t[i] * n_prime[0];
	for (j = 0; j<size; j++){
		sum = (WORD_2) t[i + j] + (WORD_2) z * (WORD_2) n[j] + (WORD_2) c;
		t[i + j] = sum;
		c = sum >> 32;
	}
	k = i + size;
	while (c != 0){
		sum = (WORD_2) t[k] + (WORD_2) c;
		t[k] = sum;
		c = sum >> 32;
		k ++;

	}
}
WORD t_acc[33];

for (j = 0; j<size+1; j++){
	t_acc[j] = t[j+size];
}


//customprint2(t_acc, ", t_acc: ", 33);


// conditional subtraction

i = 31;
c = 0;

if (t_acc[32] > 0) {
	// subtraction
	for (i = 0; i < size; i++) {
		res[i] = t_acc[i] - n[i] - c;
		if (t_acc[i] < n[i] + c) {
			c = 1;
		}
		else {
			c = 0;
		}
	}
}
else {
	while (t_acc[i] == n[i]){
		i--;
	}
	if (t[i] > n[i]){
		//subtraction
		for (i = 0; i < size; i++) {
			res[i] = t_acc[i] - n[i] - c;
			if (t_acc[i] < n[i] + c) {
				c = 1;
			}
			else {
				c = 0;
			}
		}
	}
	else {
		for (i = 0; i<size; i++){
			res[i] = t_acc[i];
		}
	}
}
}




