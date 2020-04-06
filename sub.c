/*

* sub.c
*
*  Created on: Mar 17, 2020
*      Author: r0665956
*/

#include"sub.h"

/*
Calculates res = (a - b).
a and b represent large integers stored in arrays with WORD-type elements
a is an array of length_a elements (0-elements not included), b is an array of length_b elements (0-elements not included),
res has length_a elements (0-elements not included)
with a>= b !!! (we will never use negative numbers in our implementation)
*/

void sub(WORD *res, WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 1;     		/* for subtraction, begin with carry = 1 */
    WORD i;
    WORD max = ~0;			/* wil be used to detect overflow */

    for (i=1; i<=length_b|| i<=length_a; i++) {
        res[i] = a[i] + ((WORD)~b[i]) + c ;
        c =  c ? a[i] >= (max - ((WORD)~b[i]) ) :a[i] > (max - ((WORD)~b[i]) ) ; /* compute carry by detecting overflow */
    }

    /*setting res[0] right:*/
    while (res[i] == 0  && i>0){
        i--;
    }
    res[0] = i;
}

/* Calculates a = a-b*/
void subSelf( WORD *a, WORD *b)
{
    WORD length_a = a[0];
    WORD length_b = b[0];
    WORD c = 1;     /* with subtraction, begin with carry = 1*/
    WORD i;
    WORD max = ~0;
    WORD res[SIZE] = {0};

    for (i=1; i<=length_b|| i<=length_a; i++) {
        res[i] = a[i] + ((WORD)~b[i]) + c ;
        c =  c ? a[i] >= (max - ((WORD)~b[i]) ) :a[i] > (max - ((WORD)~b[i]) ) ; /* compute carry by detecting overflow*/
    }

    /*setting res[0] right:*/
    while (res[i] == 0  && i>0){
        i--;
    }
    res[0] = i;

    copyWord(a, res);
}


void subTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    subTestHelp("300000000", "300000000", "0", &pass,  print);
    subTestHelp("200000000", "100000000", "100000000", &pass,  print);
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
    subTestHelp("B78BBFBFDBFDCD", "8AB687687",  "b78bb714738746", &pass,  print);
	subTestHelp( "59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "039587BAFEEEEBDDCC0293857023958BCDBCDBC723","59832029383bcdba838330fbebe13ed060102019305affb644c774235", &pass,  print);
	subTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
			"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
			"abcdefabcdefabcdefabcdefabbba8177b9ba8ca95673769a5763cd02762ffcca63874982887906fa60987daa9ee01ae2bf61bb42457", &pass,  print);
	subTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
			"1254789963332568dededcde9ab6876ab9d9ce9ab687a9decb8956798bcd8a976598bc8a956798cb898465878bac798465879ab7984546789a8735468", &pass,  print);
	subTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &pass,  print);
	subTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
			"fffffff", "abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222212222223", &pass,  print);
	subTestHelp("fffffff", "fffffff", "0", &pass,  print);
	subTestHelp("123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
				"111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
			"12344567801234567804012345367834502145378399999aaadddecededeceeecedeeedceeecedeeedeceeebf", &pass,  print);
	subTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
			"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
			"213a1513a51a6cda56cd1a639bbf6562a2411dee3b6a75dd6825eb7174a2fda0dddcf1910edff1afece112ce4e5854b132f523ff9", &pass,  print);


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

    subSelfTestHelp("200000000", "100000000", "100000000", &pass,  print);
    subSelfTestHelp("100000000", "100000000", "0", &pass,  print);
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
    subSelfTestHelp("B78BBFBFDBFDCD", "8AB687687",  "b78bb714738746", &pass,  print);
	subSelfTestHelp( "59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "039587BAFEEEEBDDCC0293857023958BCDBCDBC723","59832029383bcdba838330fbebe13ed060102019305affb644c774235", &pass,  print);
	subSelfTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
			"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
			"abcdefabcdefabcdefabcdefabbba8177b9ba8ca95673769a5763cd02762ffcca63874982887906fa60987daa9ee01ae2bf61bb42457", &pass,  print);
	subSelfTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
			"1254789963332568dededcde9ab6876ab9d9ce9ab687a9decb8956798bcd8a976598bc8a956798cb898465878bac798465879ab7984546789a8735468", &pass,  print);
	subSelfTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &pass,  print);
	subSelfTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
			"fffffff", "abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222212222223", &pass,  print);
	subSelfTestHelp("fffffff", "fffffff", "0", &pass,  print);
	subSelfTestHelp("123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
				"111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
			"12344567801234567804012345367834502145378399999aaadddecededeceeecedeeedceeecedeeedeceeebf", &pass,  print);
	subSelfTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
			"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
			"213a1513a51a6cda56cd1a639bbf6562a2411dee3b6a75dd6825eb7174a2fda0dddcf1910edff1afece112ce4e5854b132f523ff9", &pass,  print);
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
