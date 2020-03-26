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

/* function that returns the absolute value of the subtraction
 + keeps track of the sign of each number.
 This way, we never work with negative numbers */
WORD subtractUnsigned(WORD a, WORD b, WORD* aIsNeg, WORD bIsNeg){
    if (*aIsNeg ^ bIsNeg){
        return a+b;
    } else if(!*aIsNeg && !bIsNeg){
        if(a<b){
            *aIsNeg = 1;
            return b-a;
        }
        else{
            return a-b;
        }
    } else if(*aIsNeg && bIsNeg){
        if(a>b){
            return a-b;
        }
        else{
            *aIsNeg = 0;
            return b-a;
        }
    }
}

/* Test function for the above inverse */
void inverseTest(WORD print){
    BEGINTEST(print)
    WORD pass = 1;
    inverseTestHelp(5, 3, 2, print, &pass);
    inverseTestHelp(271, 383, 106, print, &pass);
    inverseTestHelp(64382695, 3948573, 3108589, print, &pass);
    inverseTestHelp(1, 1, 1, print, &pass);
    inverseTestHelp(1, 5, 1, print, &pass);
    inverseTestHelp(57, 278562785, 254127453, print, &pass);
    inverseTestHelp(278562785, 57, 5, print, &pass);
    inverseTestHelp(54, 8755, 3729, print, &pass);
    TEST(pass)
    ENDTEST(print)
}

void inverseTestHelp(WORD a, WORD m, WORD zExp, WORD print, WORD* pass){
    WORD z=0; // where the result will be stored
    inverse(&z, a, m);
    *pass &= zExp == z;
    if(print){
        if(*pass ){
            printf("%s - PASS: expected z=%d, got z=%d, \n", __FILE__, zExp, z);
        }
        else{
            printf("%s - FAIL: expected z=%d, got z=%d, \n", __FILE__, zExp, z);
        }
    }
}

/*
begcd.c
Ferdinand Hannequart
purpose: computing the gcd using the binary gcd algorithm. 
This is a recursive implementation
#include"begcd.h"
This function implements the binary extended gcd algorithm,
also called binary extended Euclidian algorithm.
In addition to computing gcd(x, y), 
it also computes the coefficients a and b of the Bezout's equality:
ax + by = gcd(a, b).
This algorithm has the adjective "binary" since it is a variant of 
the classical extended Euclidian algorithm: the computationnally
expensive divisions are replaced by elementary shifts and additions.
This implementation is more efficient than the classical approach 
when using multiple-precision numbers.
This implementation is based on Algorithm 14.61 
of the Handbook of Applied Cryptography
INPUT: two strictely positive integers x and y
OUTPUT: integers a, b and v = gcd(x, y)
void begcd(WORD* a, WORD* b, WORD* r, WORD x, WORD y){
    if(x <= 0 || y <= 0){
        printf("INVALID INPUT: input must be strictly positive");
    }
    WORD g = 1;
    while(~x & 1 && ~y & 1) //while x and y are both even
    {
        x = x >> 1;
        y = y >> 1;
        g = g << 1;
    }
    WORD u = x, v = y, A = 1, B = 0, C = 0,  D = 1;
    do{
        while(~u & 1) //while u is even, do the following
        {
            u = u >> 1;
            if(~A & 1 && ~B & 1) // if both A and B are even, then
            {
                A = A >> 1;
                B = B >> 1;
            }
            else
            {
                A = (A+y) >> 1;
                B = (B-x) >> 1;
            }
        }
        while(~v & 1) //while v is even, do the following
        {
            v = v >> 1;
            if(~C & 1 && ~D & 1) // if both C and D are even, then
            {
                C = C >> 1;
                D = D >> 1;
            }
            else
            {
                C = (C+y) >> 1;
                D = (D-x) >> 1;
            }        
        }
        // check which number is larger and subtract
        if(u>=v){
            u = u - v;
            A = A - C;
            B = B - D;
        }
        else{
            v = v - u;
            C = C - A;
            D = D - B;
        }
    }while(u != 0);
    // end, values are placed in the pointers:
    *a = C;
    *b = D;
    *r = (g*v);
}
Test function for the above begcd 
void begcdTest(){
    // template:
    // begcdTestHelp( x,  y, aExp, bExp, rExp)
    begcdTestHelp(1, 1, 0, 1, 1); 
    begcdTestHelp(693, 609, -181, 206, 21); //Handbook of Applied Crypto
    begcdTestHelp(609, 693, 173, -152, 21); 
    begcdTestHelp(240, 46, 14, -73, 2);    //Wiki
    begcdTestHelp(21474836, 10737418, 0, 1, 10737418); 
    begcdTestHelp(21474836, 10737416, 1, -2, 4); 
}
 Auxiliary test function for prints.
INPUT:  - x , y are the inputs of binary extended GCD algorithm
        - aExp, bExp are the expected coefficients
        - rExp is the expected gcd
OUTPUT: void
    
    WORD a=0, b=0, r=0; // where the result will be stored
    void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp){
begcd(&a, &b, &r, x, y);
    if( a == aExp  && b == bExp  && r == rExp ){
        printf("%s - PASS\n", __FILE__);
    }
    else{
        printf("%s - FAIL: expected a=%d, b=%d, gcd=%d but got a=%d, b=%d, gcd=%d\n", __FILE__, aExp, bExp, rExp, a, b, r);
    } 
}
void verifyCoefficients(WORD x, WORD y, WORD expGCD ){
    WORD a=0, b=0, r=0; // where the result will be stored
    begcd(&a, &b, &r, x, y);
    if(expGCD == r && expGCD == (a*x + b*y)){
        printf("%s - x=%d, y=%d, a=%d, b=%d, gcd=%d is a valid combination ", __FILE__, x, y, a, b, expGCD);
    }
    else{
        printf("%s - FAIL: x=%d, y=%d, a=%d, b=%d, gcd=%d is not valid", __FILE__, x, y, a, b, expGCD);
    }
}
///////////////////////////////
Ferdinand Hannequart
begcd.h
purpose: contains the prototypes, defines and includes
#ifndef DEF_BEGCD
#define DEF_BEGCD
#include"main.h"
void begcd(WORD* a, WORD* b, WORD* r, WORD x, WORD y);
void begcdTest();
void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp);
void verifyCoefficients(WORD x, WORD y, WORD expGCD );
#endif
*/
