
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
	WORD quotient[SIZE] = {0};
	if(greaterThan(n, a)){
		copyWord(res, a);

	}
	else if(equalWord(a,n) || a[0] == 0){
		copyWord(res,quotient);
	}
	else{
		if(a[0] == 1){
			smallMod(res, a , n);
		}
		else{
			division(quotient, res, a, n);
		}
	}
}

void smallMod(WORD *res, WORD *a, WORD *n){

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
    modTestHelp("2F984E58B2B","1FAFE27CFD","107A9D373", &pass,  print);
	modTestHelp("1FAFE27CFD","2F984E58B2B","1FAFE27CFD", &pass,  print);
    modTestHelp("9E6F6C70F1", "1FAFE27CFD", "0", &pass,  print);
    modTestHelp("9E6F6C7123456789", "1FAFE27CFD123456789456", "9E6F6C7123456789", &pass,  print);

    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
    		"1B", "B", &pass,  print);
    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
    		"20CF0EA1A1A5FC3A45ACEC29C2ED0B7", "150750C7822DF3796E49AD6E4B99F7F", &pass,  print);



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
