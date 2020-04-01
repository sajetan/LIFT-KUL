///*
//
// * add.c
// *
// *  Created on: Mar 17, 2020
// *      Author: r0665956
// */

#include"add.h"

/* performs addition res = a + b
INPUT:  - 2 arrays of WORDS of length SIZE ( a and b)
OUTPUT: - array of WORDS of length SIZE ( res)*/
void add(WORD *res, WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 0;             // carry
    WORD i;
    WORD max = ~0;          // for detecting overflow/carry

    for (i=1; i<=length_b|| i<=length_a; i++) {
        res[i] = a[i] + b[i] + c ; // if overflow, take back that extra bit
        c = c ? a[i] >= (max - b[i]) : a[i] > (max - b[i]);
    }
    res[i] = c; //overflow carry
    res[0] = i - (c==0); // adjust length
}

/* performs addition a = a + b
INPUT:  - 2 arrays of WORDS of length SIZE ( a and b)
OUTPUT: - the result a+b is stored in array a*/
void addSelf(WORD *a, WORD *b)
{
		WORD length_a = a[0];
		WORD length_b = b[0];
		WORD c = 0;
		WORD cNext = 0;
		WORD i;
        WORD max = ~0;

        for (i=1; i<=length_b|| i<=length_a; i++) {
            cNext = a[i] > (max - b[i]);
            a[i] = a[i] + b[i] + c ; // if overflow, take back that extra bit
            c = cNext;
		}
        a[i] = c;
		a[0] = i - (c==0);
}

/* performs addition a = a + 1
INPUT:  - 1 array of WORDS of length SIZE 
OUTPUT: - the result a+1 is stored in array a*/
void add1(WORD *a)
{
    WORD carry =1;
    WORD i=1;
    do{
        a[i] += carry;
        carry = a[i] == 0 ;
        i++;
    }while(carry);
    i--;
    a[0] += i>a[0]; 
}




void addTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    addTestHelp("0", "0", "0", &pass,  print);
    addTestHelp("0", "1", "1", &pass,  print);
    addTestHelp("1", "0", "1", &pass,  print);
    addTestHelp("2", "0", "2", &pass,  print);
    addTestHelp("0", "2", "2", &pass,  print);
    addTestHelp("1", "1", "2", &pass,  print);
    addTestHelp("FF", "0", "FF", &pass,  print);
    addTestHelp("FF", "1", "100", &pass,  print);
    addTestHelp("FF", "1000001", "1000100", &pass,  print);
    addTestHelp("Fa", "400C", "4106", &pass,  print);
    addTestHelp("8AB687687", "B78BBFBFDBFDCD", "B78BC86B447454", &pass,  print);
    addTestHelp("039587BAFEEEEBDDCC0293857023958BCDBCDBC723", "59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "59832029383bcdbaf634285bc9beba89e06290c734cdb12ffc62ed07b", &pass,  print);
    
    TEST(pass)
    ENDTEST(print)
}

void addTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    add(res, a, b);
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





void addSelfTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    addSelfTestHelp("0", "0", "0", &pass,  print);
    addSelfTestHelp("0", "1", "1", &pass,  print);
    addSelfTestHelp("1", "0", "1", &pass,  print);
    addSelfTestHelp("2", "0", "2", &pass,  print);
    addSelfTestHelp("0", "2", "2", &pass,  print);
    addSelfTestHelp("1", "1", "2", &pass,  print);
    addSelfTestHelp("FF", "0", "FF", &pass,  print);
    addSelfTestHelp("FF", "1", "100", &pass,  print);
    addSelfTestHelp("FF", "1000001", "1000100", &pass,  print);
    addSelfTestHelp("Fa", "400C", "4106", &pass,  print);
    addSelfTestHelp("8AB687687", "B78BBFBFDBFDCD", "B78BC86B447454", &pass,  print);
    addSelfTestHelp("039587BAFEEEEBDDCC0293857023958BCDBCDBC723", "59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "59832029383bcdbaf634285bc9beba89e06290c734cdb12ffc62ed07b", &pass,  print);
    
    TEST(pass)
    ENDTEST(print)
}

void addSelfTestHelp(char aChar[], char bChar[], char expChar[], WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    addSelf(a, b);
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