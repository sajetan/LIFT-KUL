/*
 * p256.h
 *
 *  Created on: 31-Mar-2020
 *      Author: sajetan
 */

#ifndef P256_H_
#define P256_H_

#include "globals.h"
#include "utilities.h"
#include "mult.h"
#include "mod.h"
#include "inverse.h"
//#include "p256_parameters.h"

/*  */
typedef uint16_t p256_word;
typedef uint64_t p256_dword;

#define P256_MAX_SIZE SIZE //8 for elements and 1 for size //think about this
#define P256_POINT_LEN 32 //8 for elements and 1 for size //think about this


/* CURVE PARAMETERS*/
extern char p256_curve_parameter_p_arr[];
extern p256_word p256_curve_parameter_p[];
extern char p256_curve_parameter_n_arr[];
extern char p256_curve_parameter_a_arr[];
extern char p256_curve_parameter_b_arr[];
extern p256_word p256_curve_parameter_b[];
extern char p256_curve_parameter_gx_arr[];
extern char p256_curve_parameter_gy_arr[];
extern char p256_curve_parameter_q_arr[];


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
p256_word pointValidOnCurve(p256_affine *in);
p256_word checkIfZero( p256_word* a);

//in affine
void pointAddAffine(p256_affine *out, p256_affine *in1, p256_affine *in2);
void pointDoubleAffine(p256_affine *out, p256_affine *in);
void pointScalarMultAffine(p256_affine *out, p256_affine *in, p256_integer k);
void pointScalarMultAffineWord(p256_affine *out, p256_affine *in, WORD* k);

//in jacobian
p256_word pointAddJacobian(p256_jacobian *out, p256_jacobian *in1, p256_jacobian *in2);
void pointDoubleJacobian(p256_jacobian *out, p256_jacobian *in);
void pointScalarMultJacobian(p256_jacobian *out, p256_jacobian *in, p256_integer k);


//only for testing
void p256TestScalarMultJacobian(); //test scalar mult jacobian
void p256TestScalarMultAffine();
void p256TestAdditionAffine();
void p256TestDoubleAffine();



#endif /* P256_H_ */
