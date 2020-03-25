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
    WORD u = m, v = a,B = 0, D = 1;
    WORD BIsNeg = 0, DIsNeg = 0;
    do{
        while(~u & 1){ //while u is even, do the following
            u = u >> 1;
            if( ~B & 1){ // if B is even, then
                B = B >> 1;}
            else{
                B = subtractUnsigned(B, m, &BIsNeg, 0) >> 1;}}

        while(~v & 1){ //while v is even, do the following
            v = v >> 1;
            if( ~D & 1){ // if  D is even, then
                D = D >> 1;}
            else{
                D = subtractUnsigned(D, m, &DIsNeg, 0) >> 1;}}

        // check which number is larger and subtract
        if(u>=v){
            u = u - v;
            B = subtractUnsigned(B, D, &BIsNeg, DIsNeg);}
        else{
            v = v - u;
            D = subtractUnsigned(D, B, &DIsNeg, BIsNeg);}

    }while(u != 0);

    // end, values are placed in the pointers:
    if(DIsNeg){
        *z = m-D;
    }
    else{
        *z = D;
    }
}

WORD subtractUnsigned(WORD a, WORD b, WORD* aIsNeg, WORD bIsNeg){
    if(!*aIsNeg && !bIsNeg){
        if(a<b){
            *aIsNeg = 1;
            return b-a;
        }
        else{
            return a-b;
        }
    }
    else if (!*aIsNeg && bIsNeg){
        return a+b;
    }
    else if (*aIsNeg && !bIsNeg){
        return a+b;
    }
    else if(*aIsNeg && bIsNeg){
        if(a>b){
            return a-b;
        }
        else{
            *aIsNeg = 0;

            return b-a;
        }
        *aIsNeg = a>b;
        return a-b;
    }


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
    inverseTestHelp(54, 8755, 3729);

}

void inverseTestHelp(WORD a, WORD m, WORD zExp){
    WORD z=0; // where the result will be stored
    inverse(&z, a, m);
    if(zExp == z ){
        printf("%s - PASS: expected z=%d, got z=%d, \n", __FILE__, zExp, z);
    }
    else{
        printf("%s - FAIL: expected z=%d, got z=%d, \n", __FILE__, zExp, z);

    }


}