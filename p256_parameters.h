/*
 * p256_parameters.h
 *
 *  Created on: 02-Apr-2020
 *  Author: sajetan
 */

#ifndef P256_PARAMETERS_H_
#define P256_PARAMETERS_H_

#include "p256.h"

/* -------------------------------------------------------CURVE PARAMETERS ---------------------------------------*/

/* Curve: p 256 = 2 256 − 2 224 + 2 192 + 2 96 − 1 */
char p256_curve_parameter_p_arr[]  = "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff";

//p256_word p256_curve_parameter_p[]  = {0x00000008,0xffffffff,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,0x00000001,0xffffffff};
p256_word p256_curve_parameter_p[]  = {0x0010,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0001,0x0000,0xffff,0xffff};
//p256_word p256_curve_parameter_p[]  = {0x20,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xff,0xff,0xff,0xff};


char p256_curve_parameter_n_arr[]  = "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551";
//p256_word p256_curve_parameter_n[]  = {0x00000008,0xfc632551,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,0x00000000,0xffffffff};

/* a = p 256 − 3 */
char p256_curve_parameter_a_arr[]  = "ffffffff00000001000000000000000000000000fffffffffffffffffffffffc";
//p256_word p256_curve_parameter_a[]  = {0x00000008,0xfffffffc,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,0x00000001,0xffffffff};

char p256_curve_parameter_b_arr[]  = "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b";
//p256_word p256_curve_parameter_b[]  = {0x00000008,0x27d2604b,0x3bce3c3e,0xcc53b0f6,0x651d06b0,0x769886bc,0xb3ebbd55,0xaa3a93e7,0x5ac635d8};
p256_word p256_curve_parameter_b[] ={0x0010,0x604b,0x27d2,0x3c3e,0x3bce,0xb0f6,0xcc53,0x06b0,0x651d,0x86bc,0x7698,0xbd55,0xb3eb,0x93e7,0xaa3a,0x35d8,0x5ac6};


/* base points Gx, Gy */
char p256_curve_parameter_gx_arr[] = "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296";
//p256_word p256_curve_parameter_gx[] = {0x00000008,0xd898c296,0xf4a13945,0x2deb33a0,0x77037d81,0x63a440f2,0xf8bce6e5,0xe12c4247,0x6b17d1f2};

char p256_curve_parameter_gy_arr[] = "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5";
//p256_word p256_curve_parameter_gy[] = {0x00000008,0x37bf51f5,0xcbb64068,0x6b315ece,0x2bce3357,0x7c0f9e16,0x8ee7eb4a,0xfe1a7f9b,0x4fe342e2};

/* Order q of the point G */
char p256_curve_parameter_q_arr[] = "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551";
//p256_word p256_curve_parameter_q[]  = {0x00000008,0xfc632551,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,0x00000000,0xffffffff};

/* ---------------------------------------------------------------------------------------------------------------*/



#endif /* P256_PARAMETERS_H_ */
