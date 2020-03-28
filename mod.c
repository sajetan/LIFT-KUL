
/*

 * mod.c

 *
 *  Created on: Mar 17, 2020
 *      Author: LIEN
 */

#include"mod.h"

// Calculates res = a mod n
// a and n represent large integers stored in arrays with WORD-type elements
// a is an array of length_a elements (0-elements not included), n is an array of length_n elements (0-elements not included),
// res has min{length_a,length_n} - elements (0-elements not included)
void mod(WORD *res, WORD *a, WORD *n){

	while (greaterThan(a,n) == 1){
		subSelf(a,n);
	}

	if (equalWord(a,n) == 1){
		sub(res,a,n);
	}

	else{
		copyWord(res,a);
	}
}


void modTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    modTestHelp("0", "0", "0", &pass,  print);
    modTestHelp("1", "1", "0", &pass,  print);
    modTestHelp("0", "1", "0", &pass,  print);
    modTestHelp("1", "2", "1", &pass,  print);
    modTestHelp("2", "1", "0", &pass,  print);
    modTestHelp("8", "2", "0", &pass,  print);
    modTestHelp("9", "2", "1", &pass,  print);
    modTestHelp("FF", "1", "0", &pass,  print);
    modTestHelp("1", "100", "1", &pass,  print);
    modTestHelp("10100", "FF", "2", &pass,  print);
    modTestHelp("2F984E58B2B", "1FAFE27CFD","107A9D373", &pass,  print);
	  modTestHelp("1FAFE27CFD","2F984E58B2B","1FAFE27CFD", &pass,  print);
    modTestHelp("9E6F6C70F1", "1FAFE27CFD", "0", &pass,  print);


    TEST(pass)
    ENDTEST(print)
}

void modTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    mod(res, a, b);
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
