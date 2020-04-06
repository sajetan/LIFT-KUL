/*

* add.c
*
*  Created on: Mar 17, 2020
*      Author: r0665956
*/

#include"add.h"

/* performs addition res = a + b
INPUT:  - 2 arrays of WORDS of length SIZE ( a and b)
OUTPUT: - array of WORDS of length SIZE ( res)*/
void add(WORD *res, WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 0;             /* carry */
    WORD i;
    WORD max = ~0;          /* for detecting overflow/carry */

    for (i=1; i<=length_b|| i<=length_a; i++) {
        res[i] = a[i] + b[i] + c ; // if overflow, take back that extra bit
        c = c ? a[i] >= (max - b[i]) : a[i] > (max - b[i]);
    }
    res[i] = c; /*overflow carry*/
    res[0] = i - (c==0); /* adjust length */
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
            cNext = c ? a[i] >= (max - b[i]) : a[i] > (max - b[i]);
            a[i] = a[i] + b[i] + c ; /* if overflow, take back that extra bit*/
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
    addTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
    		"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
    		"abcdefabcdefabcdefabcdefabe0372ad0f100420cdef3278354095a69c5ac59af4149259152294a33d235c37a7aab3ae694a6920455", &pass,  print);
    addTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
    		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
    		"125478996333256a34343233f00bdcc00f2f23f00bdcff342100cdf10345020edd1034020cdf1043031dff212546131dff21345131e102345642f1022", &pass,  print);
    addTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    		"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    		"100000000000000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe", &pass,  print);
    addTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
    		"fffffff", "abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222232222221", &pass,  print);
    addTestHelp("fffffff", "fffffff", "1ffffffe", &pass,  print);
    addTestHelp("111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
    		"123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
    		"12346789a23456789a26234567589a5672436759a5bbbbbccd0000f10100f110f10110ff110f1011100f110fd", &pass,  print);
    addTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
    		"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
    		"213a1513a51a6cda56cd1a6579ea2f60136b1b5f5f672cbba592a99c1474e16bba14daa6bb838cfca515de699b13978c4b82d8913", &pass,  print);


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
    addSelfTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
        		"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
        		"abcdefabcdefabcdefabcdefabe0372ad0f100420cdef3278354095a69c5ac59af4149259152294a33d235c37a7aab3ae694a6920455", &pass,  print);
	addSelfTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
			"125478996333256a34343233f00bdcc00f2f23f00bdcff342100cdf10345020edd1034020cdf1043031dff212546131dff21345131e102345642f1022", &pass,  print);
	addSelfTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"100000000000000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe", &pass,  print);
	addSelfTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
			"fffffff", "abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222232222221", &pass,  print);
	addSelfTestHelp("fffffff", "fffffff", "1ffffffe", &pass,  print);
	addSelfTestHelp("111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
			"123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
			"12346789a23456789a26234567589a5672436759a5bbbbbccd0000f10100f110f10110ff110f1011100f110fd", &pass,  print);
	addSelfTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
			"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
			"213a1513a51a6cda56cd1a6579ea2f60136b1b5f5f672cbba592a99c1474e16bba14daa6bb838cfca515de699b13978c4b82d8913", &pass,  print);

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
