/*
 * mod.c
 *
 *  Created on: Mar 17, 2020
 *      Author: LIEN
 */

#include"mod.h"

/* 
Calculates res = a mod n
a and n represent large integers stored in arrays with WORD-type elements
a is an array of length_a elements (0-elements not included), n is an array of length_n elements (0-elements not included),
*/

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

void mod_sub(WORD *res, WORD *a, WORD *b, WORD *n){
	WORD temp[SIZE] = {0};
    sub(temp,a, b);
    if (a[0]==0 && b[0]==0) return;

    if ( greaterThan(b,a) || equalWord(b,a)){
            
            add(res,temp,n);
            
    }
    else
    	copyArrayWithSize(res,temp);

    res[0]=n[0];
}

/*
calculates res = a mod n, when there is only a small difference between a and n,
is used in the general mod function since the division function cannot handle an input a with a[0] = 1
*/

void smallMod(WORD *res, WORD *a, WORD *n){
	WORD c[SIZE] = {0};
	copyWord(c, a);
	while (greaterThan(c,n) == 1){
		subSelf(c,n);
	}

	if (equalWord(c,n) == 1){
		sub(res,c,n);
	}

	else{
		copyWord(res,c);
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
    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
        		"20CF0EA1A1A5FC312345644897894561236546468abbbbcccd54648eef45648546665161fe54561d516e1f54e65f4e54d5e615165d1e51561A45ACEC29C2ED0B7",
        		"c811724331c0bf009e0c7085d79dc16b2ab95289050213a33619f9757c6e357a3353e6d9c1e1797f84a3e0d5f3c2ab2f5ad5e0ba7f3f219f3c02cea7f8b9c88e", &pass,  print);
    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
        		"20CF0EA1A1A5FC3A45ACEC29C123456486465162321564894864564564564561a615c56c1a561c5a61ca561ca56c1a51c5a1c54a6c48a4c65c4a4c6ca8465a4ca654a684a864ca65c4a564c56ac42ED0B7",
        		"f0b22c70313f84e6528979c359f36917bdff4b365f81cf5d40af394eab27dd440cff87036d8a972fe2f9e8fdaa8caf778b7d5014436cf1e73f59834d995a1f2c31", &pass,  print);
    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
        		"F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
        		"0", &pass,  print);
    modTestHelp("12354",
        		"F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
        		"12354", &pass,  print);
    modTestHelp("F0B22C70313F84E6528979C359F36917BDFF4B365F81CF5D40AF394EAB27DD440CFF87036D8A972FE2F9E8FDAA8CAF778B7D5014436CF1E73F59834D995A1F2C31",
    			"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff00fff",
        		"b22c70313f84e6528979c359f36917bdff4b365f81cf5d40af394eab27dd440cff87036d8a972fe2f9e8fdaa8caf778b7d5014436cf1e73f59834d995b0e3c40", &pass,  print);
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
