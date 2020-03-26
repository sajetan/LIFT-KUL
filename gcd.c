/*
Ferdinand Hannequart
gcd.c

purpose: computing the gcd using the binary gcd algorithm. 
This is a recursive implementation based on the one of the wikipedia page.
*/
#include"gcd.h"

WORD gcd(WORD u, WORD v){

    // if inputs are initially both 0, default output is 0
    // otherwise the program will ALWAYS end here
    if(u == v){
        return u;
    }

    // happens only if v is initially 0??
    if(u == 0){
        return v;
    }

    // happens only if v is initially 0??
    if(v == 0){
        return u;
    }

    if (~u & 1){            // u even
        if (~v & 1){        // u even, v even
            return gcd(u>>1, v>>1)<<1;
        }
        else {               //u even, v odd
            return gcd(u>>1, v);
        }
    }
    else{
        if (~v & 1){        // u odd, v even
        return gcd(u, v>>1);
        }
    }

    // both are odd, we must find which one is greater and subtract
    if (u>v){
        return gcd ((u-v)>>1, v);
    }
    else{
        return gcd ((v-u)>>1, u);
    }
}

void gcdTest(WORD print)
{
    BEGINTEST(print)
    WORD pass = 1;
    // corner cases
    gcdTestPrint(0, 0, 0, print, &pass);
    gcdTestPrint(1, 0, 1, print, &pass);
    gcdTestPrint(2, 0, 2, print, &pass);
    gcdTestPrint(0, 1, 1, print, &pass);
    gcdTestPrint(0, 2, 2, print, &pass);
    gcdTestPrint(249856,249856 , 249856, print, &pass);

    // other cases
    gcdTestPrint(23, 17, 1, print, &pass);
    gcdTestPrint(20, 36, 4, print, &pass);
    gcdTestPrint(60, 36, 12, print, &pass);
    gcdTestPrint(123947628, 563487258, 6, print, &pass);
    gcdTestPrint(568790880, 58, 2, print, &pass);
    TEST(pass)
    ENDTEST(print)
}

void gcdTestPrint(WORD u, WORD v, WORD exp, WORD print, WORD* pass){
    *pass &= exp == gcd(u, v);
    if(print){
        if(*pass){
            printf("Test OK: The gcd of %d and %d is %d\n", u, v, exp);
        } else{
            printf("FAIL: The gcd of %d and %d is %d, expected %d\n", u, v, gcd(u, v), exp);
        }
    }
}
