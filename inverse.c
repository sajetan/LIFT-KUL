/*
Ferdinand Hannequart
inverse.c
*/

#include"inverse.h"


/* This function computes the inverse z of a mod p, where
az mod m = 1, m is odd, a and m are coprime (i.e. gcd(a, m) = 1).

This function is essentially the binary extended gcd algorithm,
also called binary extended Euclidian algorithm.
The only difference is that since we are only looking for an inverse
and that we know that gcd(a, m) = 1, some unnecessary computations
are left out.

This algorithm has the adjective "binary" since it is a variant of 
the classical extended Euclidian algorithm: the computationnally
expensive divisions are replaced by elementary shifts and additions.
This implementation is more efficient than the classical approach 
when using multiple-precision numbers.

This implementation is based on Algorithm 14.61 
of the Handbook of Applied Cryptography.
As mentioned in note 14.64, the values A and C are not computed.

INPUT: two strictely positive integers a and p, p odd, gcd(a,m) = 1.
OUTPUT: integer z, inverse of a mod m
*/
void inverse(WORD* z, WORD a, WORD m){
    if(m <= 0 || a <= 0){
        printf("INVALID INPUT: input must be strictly positive, code will possibly get stuck in infinite loop\n");
    }
    if(~m & 1){
        printf("INVALID INPUT: m must be odd\n");
    }
    WORD u = m, v = a,B = 0, D = 1;
    do{
        while(~u & 1){ //while u is even, do the following
            u = u >> 1;
            if( ~B & 1){ // if B is even, then
                B = B >> 1;}
            else{
                B = (B-m) >> 1;}}

        while(~v & 1){ //while v is even, do the following
            v = v >> 1;
            if( ~D & 1){ // if  D is even, then
                D = D >> 1;}
            else{
                D = (D-m) >> 1;}}

        // check which number is larger and subtract
        if(u>=v){
            u = u - v;
            B = B - D;}
        else{
            v = v - u;
            D = D - B;}
    }while(u != 0);

    // end, values are placed in the pointers:
    *z = D;
    if(D<0){
        *z = D + m;}
}

/* Test function for the above inverse */
void inverseTest(){
    inverseTestHelp(5, 3, 2);
    inverseTestHelp(271, 383, 106);
    inverseTestHelp(64382695, 3948573, 3108589);
    inverseTestHelp(1, 1, 1);
    inverseTestHelp(1, 5, 1);
    inverseTestHelp(57, 278562785, 254127453);
    inverseTestHelp(278562785, 57, 5);
}

void inverseTestHelp(WORD a, WORD m, WORD zExp){
    WORD z=0; // where the result will be stored
    inverse(&z, a, m);
    if(zExp == z ){
        printf("%s - PASS: expected z=%wd, got z=%wd, \n", __FILE__, zExp, z);
    }
    else{
        printf("%s - FAIL: expected z=%wd, got z=%wd, \n", __FILE__, zExp, z);
    }


}