/*
 * p256.h
 *
 *  Created on: 31-Mar-2020
 *      Author: sajetan
 */

#ifndef P256_H_
#define P256_H_

#include "main.h"
#include "mult.h"
#include "mod.h"

/*  */
typedef uint16_t p256_word;
typedef uint64_t p256_dword;

#define P256_MAX_SIZE 64 //8 for elements and 1 for size

// All P-256 integers are non-negative and are of 32bytes long. This shall be represented as an array of 8 elements of 32bits each.
typedef struct{
	p256_word word[SIZE];
} p256_integer;


typedef struct{
	p256_word x[P256_MAX_SIZE];
	p256_word y[P256_MAX_SIZE];
} p256_affine;


typedef struct{
	p256_word x[P256_MAX_SIZE];
	p256_word y[P256_MAX_SIZE];
	p256_word z[P256_MAX_SIZE];
} p256_jacobian;


void affineToJacobian(p256_jacobian *out,p256_affine *in);
void jacobianToAffine(p256_affine *out,p256_jacobian *in);
p256_word checkIfZero( p256_word* a);
void pointAdd(p256_jacobian *out, p256_jacobian *in1, p256_jacobian *in2);
void pointDoubleJacobian(p256_jacobian *out, p256_jacobian *in);
p256_word pointValidOnCurve(p256_affine *in);
void pointScalarMultJacobian(p256_affine *out, p256_jacobian *in, p256_integer k);

#endif /* P256_H_ */
