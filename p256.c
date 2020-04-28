/*
 * p256.c
 *
 *  Created on: 31-Mar-2020
 *  Author: sajetan
 */

#include "p256.h"
#include "p256_parameters.h"
#include "test.h"
#include <stdint.h>      // for uint32_t


p256_word checkIfZero( p256_word* in) {
	p256_word i, isZero = 0;
	for (i = 1; i < P256_MAX_SIZE-1; ++i) isZero |= in[i];
	return !isZero; //if 1: then zero, else : non zero
}


// affineToJacobian returns a Jacobian Z value for the affine point (x, y). If x and
// y are zero, it assumes that they represent the point at infinity because (0, 0) is not on the any of the curves handled here.
void affineToJacobian(p256_jacobian *out,p256_affine *in) {
    if (checkIfZero(in->x) != TRUE || checkIfZero(in->y) != TRUE) {
    	copyArrayWithSize(out->x,in->x);
    	copyArrayWithSize(out->y,in->y);
    	out->z[0]=0x1;out->z[1]=0x1;
    }
    else
    	printf("flase \n");
    	return;
}



//// this reverses the Jacobian transform. If the point is ∞ it returns 0, 0.
void jacobianToAffine(p256_affine *out,p256_jacobian *in) {
	if (checkIfZero(in->z)){
		out->x[0]=0x1;out->x[1]=0x0; //point at infinity
		out->y[0]=0x1;out->y[1]=0x0; //point at inifinty
		return;
	}

	p256_integer tmpx, tmpy,z2inv,z3inv, zinv;

	initArray(tmpx.word,SIZE);
	initArray(tmpy.word,SIZE);
	initArray(z2inv.word,SIZE);
	initArray(z3inv.word,SIZE);
	initArray(zinv.word,SIZE);

	inverse(zinv.word,in->z,p256_curve_parameter_p);

	mult(z2inv.word, zinv.word,zinv.word);
	mult(tmpx.word,in->x,z2inv.word);
	mod(out->x,tmpx.word,p256_curve_parameter_p);

	mult(z3inv.word, z2inv.word,zinv.word);
	mult(tmpy.word,in->y,z3inv.word);
	mod(out->y,tmpy.word,p256_curve_parameter_p);

}


//not tested
//p256_word checkIfNegative( p256_word* a) {
//p256_word i, zero = 0;
//	for (i = 1; i < P256_MAX_SIZE-1; ++i) zero |= a[i];
//	return !zero;
//}



// check for y^2 = x^2 - 3x + b and return if point (x,y) is on curve. 1: TRUE, 0: FALSE
p256_word pointValidOnCurve(p256_affine *in){

	p256_integer x,x3,y,tmpx;
	initArray(x.word,SIZE);
	initArray(y.word,SIZE);
	initArray(tmpx.word,SIZE);
	initArray(x3.word,SIZE);

	mult(y.word, in->y,in->y);mod(y.word,y.word,p256_curve_parameter_p); //y2

	mult(x3.word, in->x,in->x);mod(tmpx.word,tmpx.word,p256_curve_parameter_p);
	mult(x3.word,x3.word,in->x);mod(x3.word,x3.word,p256_curve_parameter_p); //x3

	smallMult(tmpx.word,3,in->x); mod(tmpx.word,tmpx.word,p256_curve_parameter_p);//3x

	mod_sub(tmpx.word,x3.word,tmpx.word,p256_curve_parameter_p); //x3-3x
	add(x.word,tmpx.word,p256_curve_parameter_b); mod(x.word,x.word,p256_curve_parameter_p);//x3-3x+b

	return equalWord(x.word,y.word);
}



// doubleJacobian takes a point in Jacobian coordinates, (x, y, z), and
// returns its double, also in Jacobian form.
// See https://hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-3.html#doubling-dbl-2001-b
void pointDoubleJacobian(p256_jacobian *out, p256_jacobian *in){
	p256_integer z1z1, alpha, tmpalpha,alpha2, beta,tmpbeta, delta, gamma,tmpgamma, tmpx,tmpy, tmpz, tmpz2;

	//initArray(z1z1.word,SIZE);
	initArray(z1z1.word, SIZE);
	initArray(alpha.word,SIZE);
	initArray(tmpalpha.word,SIZE);
	initArray(alpha2.word,SIZE);
	initArray(beta.word,SIZE);
	initArray(tmpbeta.word,SIZE);
	initArray(delta.word,SIZE);
	initArray(gamma.word,SIZE);
	initArray(tmpgamma.word,SIZE);
	initArray(tmpx.word,SIZE);
	initArray(tmpy.word,SIZE);
	initArray(tmpz.word,SIZE);
	initArray(tmpz2.word,SIZE);

//    delta = Z12
	mult(delta.word, in->z,in->z);
	mod(delta.word, delta.word,p256_curve_parameter_p);

//    gamma = Y12
	mult(gamma.word,in->y,in->y);

	mod(tmpgamma.word, gamma.word,p256_curve_parameter_p);
	mod_sub(alpha.word, in->x,delta.word, p256_curve_parameter_p); //alpha = x-delta mod p
	add(tmpalpha.word, in->x,delta.word); //alpha2 = (x+delta)mod p


	mod(tmpalpha.word, tmpalpha.word,p256_curve_parameter_p);
	mult(alpha.word, alpha.word, tmpalpha.word); // alpha = (x-delta)(x+delta)
	smallMult(alpha.word, 3, alpha.word); //alpha = 3*(x-delta)*(x+delta)
	//initArray(alpha2.word,0x0, SIZE);
	mult(alpha2.word, alpha.word, alpha.word);
	mod(alpha2.word,alpha2.word,p256_curve_parameter_p ); //alpha2 % p

	//	beta = X1*gamma
	mult(beta.word,in->x,gamma.word); //beta = x*gamma


	smallMult(tmpbeta.word,8,beta.word); //8*beta
	mod(tmpbeta.word, tmpbeta.word, p256_curve_parameter_p); //8*beta %p

	mod_sub(out->x, alpha2.word, tmpbeta.word,p256_curve_parameter_p); //x3= (alpha2 - 8*beta) %p



	//computing out->z ///////////////////////////////
	add(tmpz.word, in->y,in->z ); //tmpz=y+z

	mult(tmpz2.word, tmpz.word, tmpz.word); //tmpz=(y+z)(y+z)

	mod(tmpz2.word,tmpz2.word,p256_curve_parameter_p); //tmpz=(y+z)(y+z) %p
	initArray(tmpz.word,SIZE);
	mod_sub(tmpz.word,tmpz2.word, tmpgamma.word,p256_curve_parameter_p); //tmpz=(y+z)2 - gamma

	mod_sub(out->z,tmpz.word, delta.word,p256_curve_parameter_p); //y=((y+z)2 - gamma -delta) %p



	//computing out->y ///////////////////////////////
	mult(gamma.word,gamma.word,gamma.word); mod(gamma.word,gamma.word,p256_curve_parameter_p); //gamma= gamma*gamma
	smallMult(gamma.word, 8, gamma.word); mod(gamma.word,gamma.word,p256_curve_parameter_p);//gamma=gamma*gamma*8


	smallMult(tmpbeta.word, 4, beta.word);

	mod(tmpbeta.word,tmpbeta.word,p256_curve_parameter_p);//tmpbeta = beta*4
	mod_sub(tmpy.word, tmpbeta.word, out->x,p256_curve_parameter_p);// tmpy= beta*4 - x
	mult(tmpy.word, tmpy.word, alpha.word); mod(tmpy.word,tmpy.word,p256_curve_parameter_p); //tmpy= (beta*4 - x)*alpha

	mod_sub(out->y, tmpy.word,gamma.word,p256_curve_parameter_p); //y=(beta*4 - x)*alpha - gamma*8

}



// addJacobian takes two points in Jacobian coordinates, (x1, y1, z1) and
// (x2, y2, z2) and returns their sum, also in Jacobian form.
// See https://hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-3.html#addition-add-2007-bl
p256_word pointAddJacobian(p256_jacobian *out, p256_jacobian *in1, p256_jacobian *in2){

	p256_integer z1z1, z2z2, s1, s2, u1, u2, h, i, j, r, r2, v, tmpv,tmp,tmpx,tmpy,tmpz, tmpj, tmpx123, tmpx456;

	if (checkIfZero(in1->z) == TRUE){
		copyArrayWithSize(out->x,in2->x);
		copyArrayWithSize(out->y,in2->y);
		copyArrayWithSize(out->z,in2->z);
		return 0;
	}
	if (checkIfZero(in2->z) == TRUE){
		copyArrayWithSize(out->x,in1->x);
		copyArrayWithSize(out->y,in1->y);
		copyArrayWithSize(out->z,in1->z);
		return 0;
	}

	initArray(z1z1.word,SIZE);
//	initArray(z1z1z1.word,SIZE);
	initArray(z2z2.word,SIZE);
//	initArray(z2z2z2.word,SIZE);
	initArray(s1.word,SIZE);
	initArray(s2.word,SIZE);
	initArray(u1.word,SIZE);
	initArray(u2.word,SIZE);
	initArray(h.word,SIZE);
	initArray(i.word,SIZE);
	initArray(r.word,SIZE);
	initArray(r2.word,SIZE);
	initArray(v.word,SIZE);
	initArray(tmp.word,SIZE);
	initArray(tmpj.word,SIZE);
	initArray(tmpx123.word,SIZE);
	initArray(tmpx456.word,SIZE);



	initArray(tmpv.word,SIZE);
	initArray(tmpx.word,SIZE);
	initArray(tmpy.word,SIZE);
	initArray(tmpz.word,SIZE);


	mult(z1z1.word, in1->z,in1->z);
	mod(z1z1.word,z1z1.word,p256_curve_parameter_p);
	mult(z2z2.word, in2->z,in2->z);
	mod(z2z2.word,z2z2.word,p256_curve_parameter_p);


	mult(u1.word,in1->x,z2z2.word);
	mod(u1.word,u1.word,p256_curve_parameter_p);

	mult(u2.word,in2->x,z1z1.word);
	mod(u2.word,u2.word,p256_curve_parameter_p);

	mod_sub(h.word,u2.word,u1.word, p256_curve_parameter_p); //this needs to be changed to modular subtraction


	copyArrayWithSize(i.word, h.word);
	shiftl1(i.word);

	mult(i.word,i.word, i.word);
	mult(j.word,h.word, i.word);
	mult(s1.word, in1->y,in2->z);
	mult(s1.word, s1.word, z2z2.word);
	mod(s1.word,s1.word,p256_curve_parameter_p);

	mult(s2.word, in2->y,in1->z);
	mult(s2.word, s2.word, z1z1.word);
	mod(s2.word,s2.word,p256_curve_parameter_p);

	mod_sub(r.word, s2.word, s1.word,p256_curve_parameter_p);


	if (checkIfZero(h.word) && checkIfZero(r.word)) {pointDoubleJacobian(out,in1);return 1;}

	shiftl1(r.word);

	mult(tmpv.word,u1.word, i.word);

	copyArrayWithSize(v.word, tmpv.word);
 
	mult(r2.word,r.word,r.word);
	copyArrayWithSize(tmpx.word, r2.word);
	copyArrayWithSize(tmpj.word, j.word);

	mod(j.word, j.word,p256_curve_parameter_p);
	mod(tmpx.word,tmpx.word,p256_curve_parameter_p);
	mod_sub(tmpx123.word,tmpx.word,j.word, p256_curve_parameter_p);
	copyArrayWithSize(tmpx.word, tmpx123.word);


	mod(tmpv.word, tmpv.word,p256_curve_parameter_p);
	mod_sub(tmpx456.word,tmpx.word,tmpv.word, p256_curve_parameter_p);
	copyArrayWithSize(tmpx.word, tmpx456.word);


	mod(tmpx.word,tmpx.word,p256_curve_parameter_p);

	mod_sub(out->x,tmpx.word,tmpv.word,p256_curve_parameter_p);


	/////////////////////////////////////////////////
	copyArrayWithSize(tmpy.word, r.word);
	initArray(tmpx.word,SIZE);

	copyArrayWithSize(tmpx.word, out->x);
	initArray(v.word,SIZE);
	mod_sub(v.word,tmpv.word,tmpx.word, p256_curve_parameter_p);
	mult(tmpy.word,tmpy.word, v.word);

	mult(s1.word,s1.word, tmpj.word);

	shiftl1(s1.word);

	mod(s1.word,s1.word,p256_curve_parameter_p);
	sub(out->y,tmpy.word, s1.word);
	mod(out->y,out->y, p256_curve_parameter_p);
////////////////////////////////////////////////////


	add(tmp.word,in1->z, in2->z);
	mult(tmp.word,tmp.word,tmp.word);

	sub(tmpz.word, tmp.word,z1z1.word);
	initArray(tmp.word,SIZE);
	sub(tmp.word,tmpz.word, z2z2.word);
	initArray(tmpz.word,SIZE);
	mult(tmpz.word,tmp.word, h.word);
	mod(out->z,tmpz.word, p256_curve_parameter_p);


}



void pointScalarMultJacobian(p256_jacobian *out, p256_jacobian *in, p256_integer k){
	p256_jacobian tmp ={0};
	p256_jacobian tmpout ={0};
	p256_word i, j;
	p256_word x=0;

	

	for (i=k.word[0];i>0;i--){
		x=k.word[i];

		for (j=0;j<8;j++){

			initArray(tmpout.x, P256_MAX_SIZE);
			initArray(tmpout.y, P256_MAX_SIZE);
			initArray(tmpout.z, P256_MAX_SIZE);

			pointDoubleJacobian(&tmpout,&tmp);

			if ((x&0x80)==0x80){				
				initArray(tmp.x,P256_MAX_SIZE);
				initArray(tmp.y,P256_MAX_SIZE);
				initArray(tmp.z,P256_MAX_SIZE);

				pointAddJacobian(&tmp,in, &tmpout);
			}
			else{
				copyArrayWithSize(tmp.x,tmpout.x);
				copyArrayWithSize(tmp.y,tmpout.y);
				copyArrayWithSize(tmp.z,tmpout.z);
			}
			x=x<<1;
		}
	}

	copyArrayWithSize(out->x,tmp.x);
	copyArrayWithSize(out->y,tmp.y);
	copyArrayWithSize(out->z,tmp.z);

}


void pointAddAffine(p256_affine *out, p256_affine *in1, p256_affine *in2){
	p256_jacobian inj1 ={0}; /* initializing affine */
	p256_jacobian inj2 ={0}; /* initializing affine */
	p256_jacobian outj ={0}; /* initializing affine */

	affineToJacobian(&inj1,in1); /* converting to jacobian */
	affineToJacobian(&inj2,in2); /* converting to jacobian */
	pointAddJacobian(&outj, &inj1, &inj2);

	jacobianToAffine(out, &outj); /* converting result back to affine */

}

void pointDoubleAffine(p256_affine *out, p256_affine *in){
	p256_jacobian inj ={0}; /* initializing affine */
	p256_jacobian outj ={0}; /* initializing affine */
	affineToJacobian(&inj,in); /* converting to jacobian */
	pointDoubleJacobian(&outj, &inj);
	jacobianToAffine(out, &outj); /* converting result back to affine */
}

void pointScalarMultAffine(p256_affine *out, p256_affine *in, p256_integer k){
	p256_jacobian inj ={0}; /* initializing affine */
	p256_jacobian outj ={0}; /* initializing affine */
	affineToJacobian(&inj,in); /* converting to jacobian */
	pointScalarMultJacobian(&outj, &inj, k);
	jacobianToAffine(out, &outj); /* converting result back to affine */
}


void p256TestScalarMultAffine(){
	p256_affine in ={0}; //initializing affine

	convert(in.x, p256_curve_parameter_gx_arr); //copying gx to array
	convert(in.y, p256_curve_parameter_gy_arr); //copying gy to array

	p256_integer k_arr; //use this to convert your k to k_arr having of 1byte array

	p256_affine out0={0}; //out
	convertWithSize(k_arr.word, k0, 8); //k - to always be of 1byte array
	pointScalarMultAffine(&out0, &in, k_arr); //both input and output in affine format
	printf("Ax=");print_hex(out0.x);
	printf("Ay=");print_hex(out0.y);


	p256_affine out1 ={0};
	convertWithSize(k_arr.word, k1, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out1, &in, k_arr);
	printf("Ax=");print_hex(out1.x);
	printf("Ay=");print_hex(out1.y);

	p256_affine out2 ={0};
	convertWithSize(k_arr.word, k2, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out2, &in, k_arr);
	printf("Ax=");print_hex(out2.x);
	printf("Ay=");print_hex(out2.y);

	p256_affine out3 ={0};
	convertWithSize(k_arr.word, k3, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out3, &in, k_arr);
	printf("Ax=");print_hex(out3.x);
	printf("Ay=");print_hex(out3.y);

	p256_affine out4 ={0};
	convertWithSize(k_arr.word, k4, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out4, &in, k_arr);
	printf("Ax=");print_hex(out4.x);
	printf("Ay=");print_hex(out4.y);

	p256_affine out5 ={0};
	convertWithSize(k_arr.word, k5, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out5, &in, k_arr);
	printf("Ax=");print_hex(out5.x);
	printf("Ay=");print_hex(out5.y);

	p256_affine out6 ={0};
	convertWithSize(k_arr.word, k6, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out6, &in, k_arr);
	printf("Ax=");print_hex(out6.x);
	printf("Ay=");print_hex(out6.y);

	p256_affine out7 ={0};
	convertWithSize(k_arr.word, k7, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out7, &in, k_arr);
	printf("Ax=");print_hex(out7.x);
	printf("Ay=");print_hex(out7.y);

	p256_affine out8 ={0};
	convertWithSize(k_arr.word, k8, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out8, &in, k_arr);
	printf("Ax=");print_hex(out8.x);
	printf("Ay=");print_hex(out8.y);

	p256_affine out9 ={0};
	convertWithSize(k_arr.word, k9, 8); //has to always be of 1byte array
	pointScalarMultAffine(&out9, &in, k_arr);
	printf("Ax=");print_hex(out9.x);
	printf("Ay=");print_hex(out9.y);
}

void p256TestDoubleAffine(){
	p256_affine in ={0}; //initializing affine
	p256_affine out={0}; //out
	convert(in.x,dx);
	convert(in.y,dy);
	pointDoubleAffine(&out,&in);
	printf("Dx=");print_hex(out.x);
	printf("Dy=");print_hex(out.y);
}

void p256TestAdditionAffine(){
	p256_affine in1 ={0}; //initializing affine
	p256_affine in2 ={0}; //initializing affine

	p256_affine out={0}; //out
	convert(in1.x,adx1);
	convert(in1.y,ady1);

	convert(in2.x,adx2);
	convert(in2.y,ady2);

	pointAddAffine(&out,&in1, &in2);

	printf("x=");print_hex(out.x);
	printf("y=");print_hex(out.y);
}


void p256TestScalarMultJacobian(){

	p256_affine in1 ={0}; //initializing affine

	p256_integer k_arr;

	p256_jacobian in1_j ={0}; //initiaizing jacobian

	convert(in1.x, p256_curve_parameter_gx_arr); //copying gx to array
	convert(in1.y, p256_curve_parameter_gy_arr); //copying gy to array

	affineToJacobian(&in1_j,&in1); // converting to affine

	p256_jacobian out ={0};
	convertWithSize(k_arr.word, k0, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out, &in1_j, k_arr);
	printf("1 Ax=");print_hex(out.x);
	printf("1 Ay=");print_hex(out.y);
	printf("1 Az=");print_hex(out.z);

	p256_jacobian out9 ={0};
	convertWithSize(k_arr.word, k1, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out9, &in1_j, k_arr);
	printf("2 Ax=");print_hex(out9.x);
	printf("2 Ay=");print_hex(out9.y);
	printf("2 Az=");print_hex(out9.z);

	p256_jacobian out1 ={0};
	convertWithSize(k_arr.word, k2, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out1, &in1_j, k_arr);
	printf("3 Ax=");print_hex(out1.x);
	printf("3 Ay=");print_hex(out1.y);
	printf("3 Az=");print_hex(out1.z);

	p256_jacobian out2 ={0};
	convertWithSize(k_arr.word, k3, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out2, &in1_j, k_arr);
	printf("4 Ax=");print_hex(out2.x);
	printf("4 Ay=");print_hex(out2.y);
	printf("4 Az=");print_hex(out2.z);

	p256_jacobian out3 ={0};
	convertWithSize(k_arr.word, k4, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out3, &in1_j, k_arr);
	printf("5 Ax=");print_hex(out3.x);
	printf("5 Ay=");print_hex(out3.y);
	printf("5 Az=");print_hex(out3.z);

	p256_jacobian out4 ={0};
	convertWithSize(k_arr.word, k5, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out4, &in1_j, k_arr);
	printf("6 Ax=");print_hex(out4.x);
	printf("6 Ay=");print_hex(out4.y);
	printf("6 Az=");print_hex(out4.z);

	p256_jacobian out5 ={0};
	convertWithSize(k_arr.word, k6, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out5, &in1_j, k_arr);
	printf("7 Ax=");print_hex(out5.x);
	printf("7 Ay=");print_hex(out5.y);
	printf("7 Az=");print_hex(out5.z);

	p256_jacobian out6 ={0};
	convertWithSize(k_arr.word, k7, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out6, &in1_j, k_arr);
	printf("8 Ax=");print_hex(out6.x);
	printf("8 Ay=");print_hex(out6.y);
	printf("8 Az=");print_hex(out6.z);

	p256_jacobian out7 ={0};
	convertWithSize(k_arr.word, k8, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out7, &in1_j, k_arr);
	printf("9 Ax=");print_hex(out7.x);
	printf("9 Ay=");print_hex(out7.y);
	printf("9 Az=");print_hex(out7.z);

	p256_jacobian out8 ={0};
	convertWithSize(k_arr.word, k9, 8); //has to always be of 1byte array
	pointScalarMultJacobian(&out8, &in1_j, k_arr);
	printf("10 Ax=");print_hex(out8.x);
	printf("10 Ay=");print_hex(out8.y);
	printf("10 Az=");print_hex(out8.z);
}


//int main(){
//	p256TestScalarMultJacobian(); //test scalar mult jacobian
//	p256TestScalarMultAffine();
//	p256TestAdditionAffine();
//	p256TestDoubleAffine();
//	return 0;
//}



