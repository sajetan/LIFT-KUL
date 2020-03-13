/*
 * montgomery.c
 *
 */

#include "montgomery.h"

// Calculates res = a * b * r^(-1) mod n.
// a, b, n, n_prime represent operands of size elements
// res has (size+1) elements
void montMul(uint32_t *a, uint32_t *b, uint32_t *n, uint32_t *n_prime, uint32_t *res, uint32_t size)
{
// multiplication
uint32_t i;
uint32_t c;
uint32_t j;
uint64_t sum;
uint32_t t[65] = {0};

for (i = 0; i<size; i++){
	c = 0;
	for (j = 0; j<size; j++){
		sum = (uint64_t) t[i + j] + (uint64_t) a[j] * (uint64_t) b[i] + (uint64_t) c;
		t[i + j] = sum;
		c = sum >> 32;
	}
	t[i + size] = c;
}


// reduction

uint32_t z;
uint32_t k;

for (i = 0; i<size; i++){
	c = 0;
	z = t[i] * n_prime[0];
	for (j = 0; j<size; j++){
		sum = (uint64_t) t[i + j] + (uint64_t) z * (uint64_t) n[j] + (uint64_t) c;
		t[i + j] = sum;
		c = sum >> 32;
	}
	k = i + size;
	while (c != 0){
		sum = (uint64_t) t[k] + (uint64_t) c;
		t[k] = sum;
		c = sum >> 32;
		k ++;
		 
	}
}
uint32_t t_acc[33];

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
