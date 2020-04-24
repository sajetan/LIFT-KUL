/*

 * mod2.c
 *
 *  Created on: Mar 17, 2020
 *      Author: r0665956
 */

#include"mod2.h"

// Calculates res = a mod n
// a and n represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements (0-elements not included), n is an array of length_n elements (0-elements not included),
// res has min{length_a,length_n} - elements (0-elements not included)
void mod2(WORD *res, WORD *a, WORD *n, WORD size)
{

	// if length_a > length_n => a > n
	while(a[0] > n[0]){
		sub(a,a,n, size);

	}
	//after while-loop, you are sure that a[0] <= n[0]


	// if n[0] > a[0] => n > a => res = a
	if (n[0] > a[0]){
		WORD i;
		for(i=0; i<size; i++) {
			res[i] = a[i];
		}
	return;
	}



	if (a[0] == n[0]){

		WORD z = a[0];

		//if a[z] > n[z] => a > n
		if(a[z] > n[z]){
			while (a[z] > n[z]) {
				// a =  a - n
				sub(a,a,n, size);
			}
		}
		//after while-loop, you are sure that a[z]<= n[z]

		//if a[z] < n[z] => a < n => res = a
		if (a[z] < n[z]){
			WORD i;
			for(i=0; i<size; i++) {
				res[i] = a[i];
			}
		return;
		}

		if (a[z] == n[z]){
			while (a[z]==n[z] && z!=0){
			// z = index of most significant element of a and n that are not equal
				z--;
			}


			// if z = 0 => a == n => res = 0
			if (z==0){
				WORD i;
				for(i=0; i<size; i++) {
					res[i] = 0;
				}
			return;
			}


			//if a[z] < n[z] => a < n => res = a
			if (a[z] < n[z]){
				WORD i;
				for(i=0; i<size; i++) {
					res[i] = a[i];
				}
			return;
			}

			//if a[z] > n[z] => a > n
			if (a[z] > n[z]) {
				// a =  a - n
				sub(a,a,n,size);
					}
				WORD i;
				for(i=0; i<size; i++) {
					res[i] = a[i];
				}
			return;

			}
	}

}


