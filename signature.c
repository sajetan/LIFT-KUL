/*
 * signature.c


 *
 *  Created on: Mar 30, 2020
 *      Author: Lien
 */

#include"signature.h"
#include"p256.h"



//INPUT: m is an array representing the message that needs to be signed
//INPUT: n is an array representing the modulus
//INPUT: key is an array representing the private key of the signer
//INPUT: n is an array representing the modulus
//INPUT: G_x and G_y are arrays representing the x and y coordinate of the generator
//OUTPUT: output = r|s 

//concatenating r+s and sending as the output.
void signature_gen(WORD *output, WORD *key, WORD *message, WORD *n, WORD *G_x, WORD *G_y){

	WORD i;
	WORD r_inter[SIZE] = {0};
	uint8_t k_8[SIZE] = {0};
	p256_affine point1 = {0};
	p256_integer k_struct = {0};
	uint32_t bits = 256;
	WORD e[SIZE] = {0};
	WORD k_inv[SIZE] = {0};
	WORD dr[SIZE] = {0};
	WORD a[SIZE] = {0};
	WORD b[SIZE] = {0};
	WORD sb[SIZE] = {0};
	WORD s[SIZE] = {0};
	//WORD k[SIZE]={0};
	WORD k[SIZE] = {0x0010,0x1231,0x5487,0xab25,0xfe80,0xabfd,0x58ae,0x2589,0x0001,0xfa8d,0x1010,0xaaaa,0x9870,0xafd8,0x0a0a,0x2587,0xa25f};
	WORD result[SIZE] = {0};


	p256_affine G = {0};
	copyWord(G.x, G_x);
	copyWord(G.y, G_y);


    EntropyPool pool;
	initPool(&pool);

	while(r_inter[0] == 0){

		// 1) k = random number in [1, n-1]
		//random(k, bits, &pool);  --> problem gives number of wrong amount of bits
		//WORD k[SIZE] = {0x0010,0x1231,0x5487,0xab25,0xfe80,0xabfd,0x58ae,0x2589,0x0001,0xfa8d,0x1010,0xaaaa,0x9870,0xafd8,0x0a0a,0x2587,0xa25f};
	//	copyWord(k, k_fake);
		//printf("key in sig- ");print_num_type_length(k,32,16);
		convertArray16toArray8(k_8, k);
		for(i = 0; i<=k_8[0];i++){
			k_struct.word[i] = k_8[i];
		}

		// 2) point1 = (x1, y1) = kG
//		printf("keystruct- ");print_num_type_length(k_struct.word,32,8);
		printf("\n----in signature coordinates \n");
		print_num(G.x);
		print_num(G.y);
		print_num(n);


		pointScalarMultAffine(&point1, &G, k_struct);
//	     printf("\n--In signature STEP2--X--- ");print_hex_type(point1.x,16);
//	     printf("\n--In signature STEP2- Y---- ");print_hex_type(point1.y,16);

		// 3) r_inter = x1 mod n
		mod(r_inter, point1.x, n);
//		printf("\n--In signature STEP3----- ");print_hex_type(r_inter,16);

	}

	// 4) if (r_inter==0): go back to step 1)
	//    else: r = r_inter

	//copyWord(r, r_inter);



	// 1) e = hash(m)
	hash(e, message, 256);
//	printf("\n--In signature STEP1-- e--- ");print_hex_type(e,16);
	// 2) s = [k^-1 * (e + dr)] mod n = [(k^-1 mod n) * [(e + dr) mod n] ] mod n

	inverse(k_inv, k, n);      // k_inv = k^-1 mod n

//	printf("\n--In signature STEP2--kInv--- ");print_hex_type(k_inv,16);
	printf("\n--In signature STEP2--key--- ");print_hex_type(key,16);
	printf("\n--In signature STEP2--r_inter--- ");print_hex_type(r_inter,16);

	mult(dr,key,r_inter);
	mod(dr, dr, n);		//dr = d*r

	printf("\n--In signature STEP2--dr--- ");print_hex_type(dr,16);
	add(a, e, dr);	// a = e + dr

	mod(b, a, n);	//b = a mod n

	printf("\n--In signature STEP2--b--- ");print_hex_type(b,16);

	//sb = k_inv * b
	mult(sb, k_inv, b);
	mod(s, sb, n);

	printf("\n--In signature STEP2--s--- ");print_hex_type(s,16);

	//r || s, this r and s will be extracted and during verification

    printf("\n--In signature s----- ");print_hex_type(s,16);
    printf("\n--In signature r----- ");print_hex_type(r_inter,16);

    for(i=1;i<=16;i++){
        result[i] = r_inter[i];
        result[i+16] = s[i];
    }

    result[0]=s[0]+r_inter[0];
     printf("\n--In signature r+s----- ");print_hex_type(result,16);
    copyArrayWithSize(output, result);
//     printf("\n--In [signature r+s output = ]----- ");print_hex_type(output,16);

}



//INPUT: input = r|s 
//OUTPUT: valid == 0 if signature is not valid, valid == 1 if signature is valid
//WORD sig_ver(WORD *input, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y)  {
WORD sig_ver(WORD *data, WORD *n, WORD *m, WORD *G_x, WORD *G_y, WORD *Q_x, WORD *Q_y)  {
	WORD i;
	WORD e[SIZE] = {0};
	WORD c[SIZE] = {0};
	WORD u1[SIZE] = {0};
	WORD u2[SIZE] = {0};
	WORD r[SIZE] = {0};
	WORD s[SIZE] = {0};

	p256_affine Q = {0};
	p256_affine G = {0};

	p256_affine point_res = {0};

	uint8_t u1_8[SIZE] = {0};
	p256_affine point1 = {0};
	p256_integer u1_struct = {0};

	uint8_t u2_8[SIZE] = {0};
	p256_affine point2 = {0};
	p256_integer u2_struct = {0};

	WORD v[SIZE] = {0};

	//extracting r and s from input

	    printf("data %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x  ",data[0],data[1],data[2],data[3],data[4], data[5],data[6],data[7],data[8],data[9]);
	    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x ",data[10],data[11],data[12],data[13],data[14], data[15],data[16],data[17],data[18],data[19]);
	    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x",data[20],data[21],data[22],data[23],data[24], data[25],data[26],data[27],data[28],data[29]);
	    printf(" %04x %04x %04x  \n",data[30],data[31],data[32]);



//	printf("\n--In [signature verify input = ]----- ");print_hex_type(input,16);
	r[0] = 16;
	s[0] = 16;

	for(i=1;i<=16;i++){
		r[i] = data[i];
		s[i] = data[i+16];
    }

	printf("\n----in signature coordinates originall\n");
	print_num(G_x);
	print_num(G_y);



	printf("\n--In [signature verify Qx = ]----- ");print_hex_type(Q_x,16);
	printf("\n--In [signature verify Qy = ]----- ");print_hex_type(Q_y,16);


	printf("\n--In [signature verify r = ]----- ");print_hex_type(r,16);
	printf("\n--In [signature verify s = ]----- ");print_hex_type(s,16);

	// 1) if r not in [0,n-1]: signature is invalid, else: go to step 2)

	if ( greaterThan(r, n) || equalWord(r, n) ){     // if (r > n or r = n): signature invalid
		printf("\n--signatrue invalied \n");
		return 0;
	}

	// 2)if s not in [0,n-1]: signature is invalid, else: go to step 3)

	if (greaterThan(s, n) || equalWord(s, n)){		// if (s > n or s = n): signature invalid
		printf("\n--signatrue invalied \n");
		return 0;
	}

	// 3) e = hash(m)
	hash(e, m, 256);

	printf("\n--In [signature verify hash(m) = ]----- ");print_hex_type(e,16);



	// 4) c = s^-1 mod n
	inverse(c, s, n);

	printf("\n--In [signature verify s^-1modn = ]----- ");print_hex_type(c,16);


	// 5) u1 = ec mod n, u2 = rc mod n
	mult(u1, c, e);
	mod(u1, u1, n);

	printf("\n--In [signature verify u1 = ]----- ");print_hex_type(u1,16);


	mult(u2, r, c);
	mod(u2, u2, n);

	printf("\n--In [signature verify u2 = ]----- ");print_hex_type(u2,16);


	// 6) point_res = (x,y) = u1G + u2Q

	convertArray16toArray8(u1_8, u1);
	for(i = 0; i<=u1_8[0]; i++){
		u1_struct.word[i] = u1_8[i];
	}

	convertArray16toArray8(u2_8, u2);
	for(i = 0; i<=u2_8[0]; i++){
		u2_struct.word[i] = u2_8[i];
	}

	printf("\n--In [signature verify u1struct = ]----- ");print_hex_type(u1_struct.word,16);
	printf("\n--In [signature verify u2struct = ]----- ");print_hex_type(u2_struct.word,16);


	// point1 = (x_1,y_1) = u1G
	copyWord(G.x, G_x);
	copyWord(G.y, G_y);
	pointScalarMultAffine(&point1, &G, u1_struct);



	printf("\n----in signature coordinates \n");
	print_num(G.x);
	print_num(G.y);

	printf("\n--In [signature verify point1x = ]----- ");print_hex_type(point1.x,16);
	printf("\n--In [signature verify point1y = ]----- ");print_hex_type(point1.y,16);


	// point2 = (x_2,y_2) = u2Q
	copyWord(Q.x, Q_x);
	copyWord(Q.y, Q_y);
	pointScalarMultAffine(&point2, &Q, u2_struct);

	printf("\n--In [signature verify point2x = ]----- ");print_hex_type(point2.x,16);
	printf("\n--In [signature verify point2y = ]----- ");print_hex_type(point2.y,16);


	//point_res = point1 + pont2
	pointAddAffine(&point_res, &point1, &point2);

	printf("\n--In [signature verify point_sresx = ]----- ");print_hex_type(point_res.x,16);
	printf("\n--In [signature verify point_resy = ]----- ");print_hex_type(point_res.y,16);

	printf("\n--In [signature verify point_resx compare with r = ]----- ");print_hex_type(r,16);

	// 7) if point_res is the infinity point: signature invalid, else: step 8)
	if (pointValidOnCurve(&point_res) == 0){
		return 0;
	}

	// 8) v = x mod n
	mod(v, point_res.x, n);


	//9) if (v==r): valid = 1, else: valid = 0;
	if (equalWord(v, r)){
		return 1;
	}
	else{
		return 0;
	}


}



void signatureTest(){
	//1 OK
    signatureTestHelp("2a61a0703860585fe17420c244e1de5a6ac8c25146b208ef88ad51ae34c8cb8c",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",		//key
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",		//Q_x
    		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",1);		//Q_y
	//2 OK
    signatureTestHelp("2a61a0703860585fe17420c2448c25146b208ef88ad51ae34c8cb8c",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",		//Q_x
    		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",1);		//Q_y
	//3 OK
    signatureTestHelp("0",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",		//Q_x
    		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",1);		//Q_y
	//4 OK
    signatureTestHelp("111112222",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",		//Q_x
    		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",1);		//Q_y
	//5 OK
    signatureTestHelp("111112222",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e152bce33576b315ececbb6406837bf51f5",		//G_y
    		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",		//Q_x
    		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",0);		//Q_y
	//6 OK
    signatureTestHelp("2a61a07038605420c244e1de5a6ac8c25146b208ef88ad51ae34c8cb8c",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"2a61a0703860585fe17420c244e1de5a6ac8c25146b208ef88ad51ae34c8cb8c",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"e1aa7196ceeac088aaddeeba037abb18f67e1b55c0a5c4e71ec70ad666fcddc8",		//Q_x
    		"d7d35bdce6dedc5de98a7ecb27a9cd066a08f586a733b59f5a2cdb54f971d5c8",1);		//Q_y
    //7 OK
    signatureTestHelp("111",  //m
    		"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",   		//n
    		"f257a192dde44227b3568008ff73bcf599a5c45b32ab523b5b21ca582fef5a0a",		//d
    		"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",		//G_x
    		"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",		//G_y
    		"d2e01411817b5512b79bbbe14d606040a4c90deb09e827d25b9f2fc068997872",		//Q_x
    		"503f138f8bab1df2c4507ff663a1fdf7f710e7adb8e7841eaa902703e314e793",1);		//Q_y
}




void signatureTestHelp(char mChar[], char nChar[], char keyChar[], char G_xChar[], char G_yChar[], char Q_xChar[], char Q_yChar[], WORD expected){

    WORD m[SIZE] = {0};
    WORD n[SIZE] = {0};
	WORD key[SIZE] = {0};
    WORD G_x[SIZE] = {0};
    WORD G_y[SIZE] = {0};
    WORD Q_x[SIZE] = {0};
    WORD Q_y[SIZE] = {0};
    WORD k[SIZE] = {0};
    WORD output[SIZE] = {0};
    WORD valid = 0;

    convert(m, mChar);
    convert(n, nChar);
    convert(key, keyChar);
    convert(G_x, G_xChar);
    convert(G_y, G_yChar);
    convert(Q_x, Q_xChar);
    convert(Q_y, Q_yChar);

    printf("\n signature input message---- ");print_num(m);
    printf("\n signature Gx ---- ");print_num(G_x);
    printf("\n signature Gy ---- ");print_num(G_y);
    printf("\n signature key ---- ");print_num(key);
    printf("\n signature Qx---- ");print_num(Q_x);
    printf("\n signature Qy---- ");print_num(Q_y);


    signature_gen(output, key, m, n, G_x, G_y);
    printf("\n signature output---- ");print_num(output);
    valid = sig_ver(output, n, m, G_x, G_y, Q_x, Q_y);
    if (valid == expected){
    	printf("signature test PASS \n");
    }
    else{
    	printf("signature test FAIL \n");
    }

}
