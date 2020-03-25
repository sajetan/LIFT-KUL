/*
 * mod.c
 *
 *  Created on: Mar 17, 2020
 *      Author: r0665956
 */

#include"mod.h"

// Calculates res = a mod n
// a and n represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements, n is an array of length_n elements, res has min{length_a,length_n} - elements
void mod(WORD *res, WORD *a, WORD *n)
{
	WORD length_a = a[0];
	WORD length_n = n[0];


	//z = index of most significant non zero element of a
	WORD z = length_a;
	while(a[z] == 0){
		z--;
	}
	//y = index of most significant non zero element of n
	WORD y = length_n;
	while(n[y] == 0){
		y--;
	}


	// if z > y => a > n
	if(z>y) {

		while (z>y){
			// a = a - n
			sub(a,a,n);
			//z = index of most significant non zero element of a
			WORD z = length_a;
			while(a[z] == 0){
				z--;
			}
			//y = index of most significant non zero element of n
			WORD y = length_n;
			while(n[y] == 0){
				y--;
			}
		}
		//after while-loop, you are sure that z <= y
	}

	// if y > z => n > a => res = a
	if (y>z){
		res[0] = length_a;
		WORD i;
		for(i=1; i<=length_a; i++) {
			res[i] = a[i];
		}
	}



	if (z==y){

		//if a[z] > n[z] => a > n
		if(a[z] > n[z]){
			while (a[z] > n[z]) {
				// a =  a - n
				sub(a,a,n);
			}

		}
		//after while-loop, you are sure that a[z]<= n[z]

		//if a[z] < n[z] => a < n => res = a
		if (a[z] < n[z]){
			res[0] = length_a;
			WORD i;
			for(i=1; i<=length_a; i++) {
				res[i] = a[i];
			}
		}

		if (a[z] == n[z]){
			while (a[z]==n[z] && z!=2){
			// z = index of most significant element of a and n that are not equal
				z--;
			}


			// if z = 2 => a = n => res = 0
			if (z==2){
				res[0] = length_a;
				WORD i;
				for(i=1; i<=length_a; i++) {
					res[i] = 0;
				}

			}


			//if a[z] < n[z] => a < n => res = a
			if (a[z] < n[z]){
				res[0] = length_a;
				WORD i;
				for(i=1; i<=length_a; i++) {
					res[i] = a[i];
					}
			}

			//if a[z] > n[z] => a > n
			if (a[z] > n[z]) {
				// a =  a - n
				sub(a,a,n);
				res[0] = length_a;
				WORD i;
				for(i=1; i<=length_a; i++) {
					res[i] = a[i];
				}
			}
		}
	}
}