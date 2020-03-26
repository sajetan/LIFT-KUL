/*
 * sig_ver.c

 *
 *  Created on: Mar 25, 2020
 *      Author: r0665956
 */

#include"sig_ver.h"

//INPUT: r and s are arrays representing the received signature
//OUTPUT: valid == 0 if signature is not valid, valid == 1 if signature is valid
void sig_ver(WORD valid, WORD *r, WORD *s, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y, WORD size)  {

	// 1) if (r > n-1): signature is invalid, else: go to step 2)
	WORD res = 0;
	compare(res, r, n);     // if (r >= n): res == 1, else: res == 0
	if (res == 1){
		valid = 0;
		return;
	}

	// 2) if (s > n-1): signature is invalid, else: go to step 3)
	compare(res, s, n);   // if (s >= n): res == 1, else: res == 0
	if (res == 1){
		valid = 0;
		return;
	}

	// 3) e = hash(m)
	WORD e[size] = {0};
	hash(e, m);

	// 4) c = s^-1 mod n
	WORD c[size] = {0};
	inverse(c, s, n);

	// 5) u1 = ec mod n, u2 = rc mod n
	WORD u1[size] = {0};
	WORD u2[size] = {0};
	mont_mult(u1, e, c);
	mont_mult(u2, r, c);

	// 6) (x1,y1) = u1G + u2Q
	// (a_x,a_y) = u1G
	WORD a_x[size] = {0};
	WORD a_y[size] = {0};
	EC_mult(a_x,a_y, u1, G_x, G_y);
	// (b_x,b_y) = u2Q
	WORD b_x[size] = {0};
	WORD b_y[size] = {0};
	EC_mult(b_x,b_y, u2, Q_x, Q_y);

	WORD x1[size] = {0};
	WORD y1[size] = {0};
	EC_add(x1,y1,a_x,a_y,b_x,b_y);


	// 7) check if (x1,y1) infinity point ???

	// 8) v = x1 mod n
	WORD v[size] = {0};
	mod2(v, x1, n, size);

	//9) if (v==r): valid = 1, else: valid = 0;
	WORD a = equalWord(v, r);
	if (a == 1){
		valid = 1;
	}
	else{
		valid = 0;
	}



}
