/*


 * mult.c
 *
 *  Created on: Mar 28, 2020
 *      Author: r0665956
 */

#include"mont_mult.h"

// calculates: res = a * b
void mult(WORD *res, WORD *a, WORD *b){
	WORD i;
	WORD j;
	WORD t[SIZE] = {0};
	WORD length_a = a[0];
	WORD length_b = b[0];
	WORD_2 sum = 0;


	for (i = 1; i <= length_a; i++ ){
		WORD c = 0;
		for(j = 1; j <=length_b; j++){
			 sum = (WORD_2) t[i+j-1-1]  + (WORD_2) a[i] * (WORD_2) b[j] + (WORD_2) c;
			 t[i+j-1-1] = sum;
			 c = sum>>BIT;
		}
		t[i-1+length_b] = c;
	}

	for(i = 1; i<SIZE; i++){
	        res[i] = t[i-1];
	    }

	WORD z = SIZE-1;

	while (res[z] == 0  && z>0){
		z--;
	}
	res[0] = z;
}


//calculates: res = a * b, with a an integer of BIT bits or smaller, and b a n array

void smallMult(WORD *res, WORD a, WORD *b){
	WORD i;
	WORD j;
	WORD t[SIZE] = {0};
	WORD length_b = b[0];
	WORD_2 sum = 0;
	WORD c = 0;

	for(j = 1; j <=length_b; j++){
		 sum = (WORD_2) t[1+j-1-1]  + (WORD_2) a * (WORD_2) b[j] + (WORD_2) c;
		 t[1+j-1-1] = sum;
		 c = sum>>BIT;
	}
	t[1-1+length_b] = c;


	for(i = 1; i<SIZE; i++){
	        res[i] = t[i-1];
	    }

	WORD z = SIZE-1;

	while (res[z] == 0  && z>0){
		z--;
	}
	res[0] = z;
}




void multTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    multTestHelp("0", "0", "0", &pass,  print);
    multTestHelp("0", "1", "0", &pass,  print);
    multTestHelp("1", "0", "0", &pass,  print);
    multTestHelp("2", "1", "2", &pass,  print);
    multTestHelp("8", "2", "10", &pass,  print);
    multTestHelp("FFFFFFF", "32", "31FFFFFCE", &pass,  print);
    multTestHelp("1", "B78BBFBFDBFDCD", "B78BBFBFDBFDCD", &pass,  print);
    multTestHelp("55AA54D38E5267EEA", "2BDC545D6B5882", "EAD56DB9558315638445A3A6FAE2D4", &pass,  print);
    multTestHelp("2BDC545D6B5882", "55AA54D38E5267EEA", "EAD56DB9558315638445A3A6FAE2D4", &pass,  print);
    multTestHelp("1000000010010", "56", "56000000560560", &pass,  print);
    multTestHelp("1", "1", "1", &pass,  print);
    multTestHelp("2E7074D9C994179CB1E5058581B6AE2FA", "D83FF057724F3153C375644C0464DA386BBD0B42", "273A7BDDCE2DE40C20619F0DF353604BF38E498BD4080FDE1220BDD0CE757055CB9E14274", &pass,  print);

    TEST(pass)
    ENDTEST(print)
}

void multTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    mult(res, a, b);
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

void smallMultTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    smallMultTestHelp(0, "0", "0", &pass,  print);
    smallMultTestHelp(0, "1", "0", &pass,  print);
    smallMultTestHelp(1, "0", "0", &pass,  print);
    smallMultTestHelp(2, "1", "2", &pass,  print);
    smallMultTestHelp(8, "2", "10", &pass,  print);
    smallMultTestHelp(268435455, "32", "31FFFFFCE", &pass,  print);
    smallMultTestHelp(1, "B78BBFBFDBFDCD", "B78BBFBFDBFDCD", &pass,  print);
    smallMultTestHelp(21930, "2BDC545D6B5882", "EAD48530AADABF054", &pass,  print);
    smallMultTestHelp(24, "55AA54D38E5267EEA", "807F7F3D557B9BE5F0", &pass,  print);
    smallMultTestHelp(10, "56", "35c", &pass,  print);
    smallMultTestHelp(1, "1", "1", &pass,  print);
    smallMultTestHelp(2, "D12584987895683FF057724F3153C375644C0464DA386BBD0B42", "1A24B0930F12AD07FE0AEE49E62A786EAC89808C9B470D77A1684", &pass,  print);

    TEST(pass)
    ENDTEST(print)
}

void smallMultTestHelp(WORD a, char bChar[], char expChar[], WORD* pass, WORD print){
    WORD b[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(b, bChar);
    convert(exp, expChar);

    smallMult(res, a, b);
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
