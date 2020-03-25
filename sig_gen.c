/*
 * sig_gen.c

 *
 *  Created on: Mar 25, 2020
 *      Author: r0665956
 */


#include"sig_gen.h"


// INPUT: n is an array representing the modulus
// INPUT: G_x and G_y are arrays representing the x and y coordinate of the generator
// INPUT: size is an integer representing the number of elements in the arrays (0-elements included)
// OUTPUT: r is an array (needs to be sent together with the signature s)
// OUTPUT: k is an array
// sig_gen_pre = precomputation of the signature generation
void sig_gen_pre(WORD *r, WORD *k, WORD *n, WORD *G_x, WORD *G_y, WORD size){

	WORD r_inter[size] = {0};
	WORD i;

	while(r_inter == 0){

		 // 1) k = random number in [1, n-1]
		rng(k, n);

		// 2) (x1, y1) = kG
		WORD x1[size] = {0};
		WORD y1[size] = {0};
		EC_mult(x1, y1, k, G_x, G_y);

		// 3) r_inter = x1 mod n
		mod2(r_inter, x1, n, size);
	}

	// 4) if (r_inter==0): go back to step 1)
	//    else: r = r_inter

	copyWord(r, r_inter, size);
}


//INPUT: m is an array representing the message that needs to be signed
//INPUT: k is an array = the output k of sig_gen_pre
//INPUT: n is an array representing the modulus
//INPUT: d is an array representing the private key of the signer
//INPUT: r is an array = output of sig_gen_pre
//INPUT WORD_length is an integer = the number of bits in each element of the array
//INPUT: size is an integer representing the number of elements in the arrays (0-elements included)
//OUTPUT: s is an integer representing the signature
void sig_gen(WORD *s, WORD *m, WORD *k, WORD *n, WORD *d, WORD *r, WORD WORD_length, WORD size){

	// 1) e = hash(m)
	WORD e[size] = {0};
	hash(e, m);

	// 2) s = [k^-1 * (e + dr)] mod n = (k^-1 mod n) * [(e + dr) mod n]

	WORD k_inv[size] = {0};
	inverse(k_inv, k, n);      // k_inv = k^-1 mod n

	WORD dr[size] = {0};
	mont_mult(dr,d,r);			//dr = d*r

	WORD a[size] = {0};
	add2(a, e, dr, WORD_length, size);	// a = e + dr

	WORD b[size] = {0};
	mod2(b, a, n, size);	//b = a mod n

	//s = k_inv * b
	mont_mult(s, k_inv, b);


}
