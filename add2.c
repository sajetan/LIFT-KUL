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
    //addAndSub(res, a, b, 0);
    
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 0;             // carry
    WORD i;
    WORD max = ~0;          // for detecting overflow/carry

    for (i=1; i<=length_b && i<=length_a; i++) {
        res[i] = a[i] + b[i] + c ; // if overflow, take back that extra bit
        c = c ? a[i] >= (max - b[i]) : a[i] > (max - b[i]);
    }

    // when one of the two is entirely scanned, just copy the other + the carry
    if(i>length_b){
        while(i<=length_a){
            res[i] = a[i] + c ; 
            c =  c ? a[i] >= max :a[i] > max ;
            i++;
        }
    }
    else if(i>length_a){
        while(i<=length_b){
            res[i] = b[i]  + c ; 
            c =  c ? b[i] >= max :b[i] > max ;
            i++;
        }
    }    
    res[i] = c; //overflow carry
    res[0] = i - (c==0); // adjust length
    
}

/*
    performs addition res = a + b if subtraction = 0
    performs subtraction res = a - b if subtraction = 1
INPUT:  - 2 arrays of WORDS of length SIZE ( a and b)
OUTPUT: - array of WORDS of length SIZE ( res)*/
void addAndSub(WORD *res, WORD *a, WORD *b, WORD subtraction)
{
    
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD length = length_b > length_a ? length_b : length_a;
    WORD c = subtraction;             // carry
    WORD i;
    WORD max = ~0;          // for detecting overflow/carry
    WORD b2[SIZE] = {0};

    
    // setting MSB out of range to zero
    if(length_a>length_b){
        for(i = length_b +1; i<=length_a ; i++){
            b[i] = 0;
        }
    } else if(length_b>length_a){
        for(i = length_a +1; i<=length_b ; i++){
            a[i] = 0;
        }
    }

    copyWord(b2, b);

    if(subtraction){
        for(i = 1; i<=length ; i++){
            b2[i] = ~b2[i];
        }
    } 

    for (i=1; i<=length; i++) {
        res[i] = a[i] + b2[i] + c ; // if overflow, take back that extra bit
        c = c ? a[i] >= (max - b2[i]) : a[i] > (max - b2[i]);
    }
   
    res[i] = subtraction ? c==0 : c; //overflow carry
    
    //setting res[0] right:
    while (res[i] == 0  && i>0){
        i--;
    }
    res[0] = i;

    
}

/* performs addition a = a + b
INPUT:  - 2 arrays of WORDS of length SIZE ( a and b)
OUTPUT: - the result a+b is stored in array a*/
void addSelf(WORD *a, WORD *b)
{
    /*
    WORD res[SIZE] = {0};
    addAndSub(res, a, b, 0);
    copyWord(a, res);
    */
   
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 0;
    WORD cNext = 0;
    WORD i;
    WORD max = ~0;

    for (i=1; i<=length_b && i<=length_a; i++) {
        cNext = c ? a[i] >= (max - b[i]) : a[i] > (max - b[i]);
        a[i] = a[i] + b[i] + c ; // if overflow, take back that extra bit
        c = cNext;
    }
    // when one of the two is entirely scanned, just copy the other + the carry
    if(i>length_b){
        while(i<=length_a){
            cNext = c ? a[i] >= max  : a[i] > max ;
            a[i] +=  c ; // if overflow, take back that extra bit
            c = cNext;
            i++;
        }
    }
    else if(i>length_a){
        while(i<=length_b){
            cNext = c ? b[i] >= max  : b[i] > max ;
            a[i] = b[i] + c ; // if overflow, take back that extra bit
            c = cNext;
            i++;
        }
    }
    a[i] = c;
    a[0] = i - (c==0);
     
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
    addTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61",
                "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551", 
                "1eac28bdec52014e59be02d07b79273e05a9a1b26a0439b779955dbccc103d3b2", &pass,  print);

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