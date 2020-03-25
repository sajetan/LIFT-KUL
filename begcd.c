/*
Ferdinand Hannequart
begcd.c

purpose: computing the gcd using the binary gcd algorithm. 
This is a recursive implementation
blabla
*/
#include"begcd.h"


/* This function implements the binary extended gcd algorithm,
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
*/
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

/* Test function for the above begcd */
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

/* Auxiliary test function for prints.
INPUT:  - x , y are the inputs of binary extended GCD algorithm
        - aExp, bExp are the expected coefficients
        - rExp is the expected gcd
OUTPUT: void
 */
    
    WORD a=0, b=0, r=0; // where the result will be stored
    void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp){
begcd(&a, &b, &r, x, y);

    if( a == aExp  && b == bExp  && r == rExp ){
        printf("%s - PASS\n", __FILE__);
    }
    else{
        printf("%s - FAIL: expected a=%wd, b=%wd, gcd=%wd but got a=%wd, b=%wd, gcd=%wd\n", __FILE__, aExp, bExp, rExp, a, b, r);
    } 
}
void verifyCoefficients(WORD x, WORD y, WORD expGCD ){
    WORD a=0, b=0, r=0; // where the result will be stored
    begcd(&a, &b, &r, x, y);
    if(expGCD == r && expGCD == (a*x + b*y)){
        printf("%s - x=%wd, y=%wd, a=%wd, b=%wd, gcd=%wd is a valid combination ", __FILE__, x, y, a, b, expGCD);
    }
    else{
        printf("%s - FAIL: x=%wd, y=%wd, a=%wd, b=%wd, gcd=%wd is not valid", __FILE__, x, y, a, b, expGCD);
    }
}

