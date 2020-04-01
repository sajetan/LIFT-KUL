///*
//
// * sub.c
// *
// *  Created on: Mar 17, 2020
// *      Author: r0665956
// */

#include"sub.h"

// Calculates res = (a - b).
// a and b represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements (0-elements not included), b is an array of length_b elements (0-elements not included),
//res has length_a elements (0-elements not included)
// with a>= b !!! (we will never use negative numbers in our implementation)
void sub(WORD *res, WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 1;     		// for subtraction, begin with carry = 1
    WORD i;
    WORD max = ~0;			// wil be used to detect overflow 

    for (i=1; i<=length_b|| i<=length_a; i++) {
        res[i] = a[i] + ((WORD)~b[i]) + c ; 
        c =  c ? a[i] >= (max - ((WORD)~b[i]) ) :a[i] > (max - ((WORD)~b[i]) ) ; // compute carry by detecting overflow
    }
    
    //setting res[0] right:
    while (res[i] == 0  && i>0){
        i--;
    }
    res[0] = i;
}

// Calculates a = a-b
void subSelf( WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 1;     // with subtraction, begin with carry = 1
    WORD cNext = 0;
    WORD i;
    WORD max = ~0;

    for (i=1; i<=length_b|| i<=length_a; i++) {
        cNext = a[i] > (max - ((WORD)~b[i]) - c);
        a[i] = a[i] + ((WORD)~b[i]) + c ; // if overflow, take back that extra bit
        c = cNext;
    }
    
    //setting a[0] right:
    while (a[i] == 0  && i>0){
        i--;
    }
    a[0] = i;
}


void subTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    subTestHelp("0", "0", "0", &pass,  print);
    subTestHelp("1", "1", "0", &pass,  print);
    subTestHelp("1", "0", "1", &pass,  print);
    subTestHelp("2", "0", "2", &pass,  print);
    subTestHelp("2", "1", "1", &pass,  print);
    subTestHelp("1", "1", "0", &pass,  print);
    subTestHelp("FF", "0", "FF", &pass,  print);
    subTestHelp("FF", "1", "FE", &pass,  print);
    subTestHelp("100", "1", "FF", &pass,  print);
    subTestHelp("10100", "FF", "10001", &pass,  print);
    subTestHelp("7432987059342BFBFBCDFBCFDBCFDBCFDBCFDBCFD",
				 "BCDFCBFDBCFDCBFDCFDBC6298476384756938275", 
				 "68649bb07d644f3c1ed03f6d4388784b6666a3a88", &pass,  print);
	subTestHelp("CF562876982769827653429857694382797634972659",
				 "BDCFCBDFBC73458725648723656CBCDFCBDFDBCFDCDF", 
				 "11865c96dbb423fb50eebb74f1fc86a2ad9658c7497a", &pass,  print);
    subTestHelp("8AB3FD76358BCD", "FDC76234686DC", "7ad78752ef04f1", &pass,  print);

    
    TEST(pass)
    ENDTEST(print)
}

void subTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    sub(res, a, b);
    *pass &= equalWord(res, exp);
    
    if(print){
        if(*pass ){
            printf("PASS \n");
        }
        else{
            printf("-- FAIL :/ \n");
        }
        printf("expected : ");
        print_num(exp);
        printf("got      : ");
        print_num(res);
    }
}





void subSelfTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    subSelfTestHelp("0", "0", "0", &pass,  print);
    subSelfTestHelp("1", "1", "0", &pass,  print);
    subSelfTestHelp("1", "0", "1", &pass,  print);
    subSelfTestHelp("2", "0", "2", &pass,  print);
    subSelfTestHelp("2", "1", "1", &pass,  print);
    subSelfTestHelp("1", "1", "0", &pass,  print);
    subSelfTestHelp("FF", "0", "FF", &pass,  print);
    subSelfTestHelp("FF", "1", "FE", &pass,  print);
    subSelfTestHelp("100", "1", "FF", &pass,  print);
    subSelfTestHelp("7432987059342BFBFBCDFBCFDBCFDBCFDBCFDBCFD",
				 "BCDFCBFDBCFDCBFDCFDBC6298476384756938275", 
				 "68649bb07d644f3c1ed03f6d4388784b6666a3a88", &pass,  print);
    subSelfTestHelp("CF562876982769827653429857694382797634972659",
				 "BDCFCBDFBC73458725648723656CBCDFCBDFDBCFDCDF", 
				 "11865c96dbb423fb50eebb74f1fc86a2ad9658c7497a", &pass,  print);
    TEST(pass)
    ENDTEST(print)
}

void subSelfTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    subSelf(a, b);
    *pass &= equalWord(a, exp);
    
    if(print){
        if(*pass ){
            printf("PASS \n");
        }
        else{
            printf("-- FAIL :/ \n");
        }
        printf("expected : ");
        print_num(exp);
        printf("got      : ");
        print_num(a);
    }
}