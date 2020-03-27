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

INPUT: two arrays a and m representing strictely positive integers, m odd, gcd(a,m) = 1.
OUTPUT: array number z, inverse of a mod m
*/
void inverse(WORD z[], WORD a[], WORD m[]){

    WORD u[SIZE] = {0};
    WORD v[SIZE] = {0};
    WORD B[SIZE] = {0};
    WORD D[SIZE] = {0};
    WORD BIsNeg = 0;    // will keep track of sign of B
    WORD DIsNeg = 0;    // will keep track of sign of D

    copyWord(u, m);     // u = m
    copyWord(v, a);     // v = a
    convert(B, "0");    // B = 0
    convert(D, "1");    // D = 1
    
    do{                                         // do
        while(~u[1] & 1){                       //while u is even, do the following
            shiftr1(u);                         // u = u >> 1
            if( ~B[1] & 1){                     // if B is even, then
                shiftr1(B);}                    // B = B >> 1
            else{                               // else
                subUnsigned(B, m, &BIsNeg, 0);  // B = B - m
                shiftr1(B);}}                   // B = B >> 1

        while(~v[1] & 1){                       //while v is even, do the following
            shiftr1(v);                         // v = v >> 1;
            if( ~D[1] & 1){                     // if D is even then
                shiftr1(D);}                    // D = D >> 1
            else{                               // else
                subUnsigned(D, m, &DIsNeg, 0);  // D = D - m
                shiftr1(D);}}                   // D = D >> 1

        if(greaterThan(u, v) || equalWord(u, v)){// if u >= v
            subSelf(u, v);                      // u = u - v
            subUnsigned(B, D, &BIsNeg, DIsNeg);}// B = B - D
        else{                                   //else
            subSelf(v, u);                      // v = v - u
            subUnsigned(D, B, &DIsNeg, BIsNeg); // D = D - B
        }
    }while(u[0] != 0 || (u[0]!=1 && u[1]!=0));  // while u different from 0

    if(DIsNeg){                                 
        sub(z, m, D);                           // if D negative: z = D + m (since D contains
    }                                           // actually a positive numbern we compute z = m-D)
    else{
        copyWord(z , D);                        // otherwise: z = D
    }
}

/* function that returns the absolute value of the subtraction
 and keeps track of the sign of each number.

 Essentially it yields a = |a-b| and indicates if a became negative or not
 through the aIsNeg pointer.

 This way, we never work with negative numbers.
 bIsNeg is not a pointer, since b's sign remains unchanged*/
void subUnsigned(WORD a[], WORD b[], WORD* aIsNeg, WORD bIsNeg){
    WORD result[SIZE] = {0};            // where the result will be stored temporarely
    
    if (*aIsNeg ^ bIsNeg){              // case 1: opposite signs
        add(result, a, b);
    } 
    
    else if(!*aIsNeg && !bIsNeg){       // case 2: both positive
        if(greaterThan(b, a)){
            *aIsNeg = 1;
            sub(result, b, a);          // b-a;
        }
        else{
            sub(result, a, b);          // a-b;
        }

    } else if(*aIsNeg && bIsNeg){       // case 3: both negative
        if(greaterThan(a, b)){
            sub(result, a, b);          // a-b;
        }
        else{
            *aIsNeg = 0;
            sub(result, b, a);          // b-a;
        }
    }
    copyWord(a, result);                // copy result to pointer
}

/* Test function for the inverse */
void inverseTest(WORD print){
    BEGINTEST(print)
    WORD pass = 1;
    //inverseTestHelp("5", "3", "2", print, &pass);
    inverseTestHelp("10f", "17f", "6a", print, &pass);
    inverseTestHelp("3d666e7", "3c401d", "2f6eed", print, &pass);
    inverseTestHelp("1", "1", "1", print, &pass);
    inverseTestHelp("1", "5", "1", print, &pass);
    inverseTestHelp("39", "109a87e1", "f25ad5d", print, &pass);
    inverseTestHelp("109a87e1", "39", "5", print, &pass);
    inverseTestHelp("36", "2233", "e91", print, &pass);
    inverseTestHelp("253af5dfef54b657c4", 
                    "d390009f057907971c18dbcb", 
                    "56fdc5ec0705d17ff0c36db6", print, &pass);

    /*
        inverseTestHelp("5", "3", "2", print, &pass);
    inverseTestHelp("271", "383", "106", print, &pass);
    inverseTestHelp("64382695", "3948573", "3108589", print, &pass);
    inverseTestHelp("1", "1", "1", print, &pass);
    inverseTestHelp("1", "5", "1", print, &pass);
    inverseTestHelp("57", "278562785", "254127453", print, &pass);
    inverseTestHelp("278562785", "57", "5", print, &pass);
    inverseTestHelp("54", "8755", "3729", print, &pass);
    */
    TEST(pass)
    ENDTEST(print)
}

void inverseTestHelp(char aChar[], char mChar[], char expChar[], WORD print, WORD* pass){
    WORD a[SIZE] = {0};
    WORD m[SIZE] = {0};
    WORD exp[SIZE] = {0};
    WORD z[SIZE]={0}; // where the result will be stored

    convert(a, aChar);
    convert(m, mChar);
    convert(exp, expChar);

    inverse(z, a, m);
    *pass &= equalWord( exp,z);
       
    if(print){
        if(equalWord( exp,z) ){
            printf("PASS \n");
        }
        else{
            printf("-- FAIL :/ \n");
        }
        printf("expected : ");
        print_num(exp);
        printf("got      : ");
        print_num(z);
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