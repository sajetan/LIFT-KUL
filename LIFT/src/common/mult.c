/*

 * mult.c
 *
 *  Created on: Mar 28, 2020
 *      Author: r0665956
 */

#include"mult.h"


/*
calculates res = a*b, with a and b arrays
 */
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


/*
calculates: res = a * b, with a an integer of BIT bits or smaller, and b an array
 */
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
	multTestHelp("8AB687687", "B78BBFBFDBFDCD", "637433cd522e6dc9e47551b", &pass,  print);
	multTestHelp("039587BAFEEEEBDDCC0293857023958BCDBCDBC723", "59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "140d227bc052d1c0b030e4f9ca8b7cb17c6e77218f60bac2cb803a84864f20d3be00eec712259a4c841d09997470e5af08", &pass,  print);
	multTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
			"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
			"c447d5b4baf128dbf2976b86b80e8064037045534e5e0cea343ad5f170adce295ce30302af325eb760d80c1a0cbc1886d0ec68c528c58edd1eaa1a1d6b70e5819635c4ac2c0df654768259930bb46d4b214dd393c41f043f537e36ce18baa", &pass,  print);
	multTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
			"c385066422218f1065bafb0d8eb76b8edaedf0317d2d55e22aeaee849e8acd45bfd3ba688ff5021534830528cb0c452e0afb93a5296e93d71c5e0a0f1c2a3b8c548728eebc256039214deac9ba9b3ece3071ee71150105ff60ee045eababeb13228c912273bee039335b24c62030ef691", &pass,  print);
	multTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", &pass,  print);
	multTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
			"fffffff", "abc1563bcb70efa9ece3551ec0bf3202ce8f0f22e1a11e0051decf6e0b5b000869a096101dff79dece13bfffff9dbd0effffffffdddddde", &pass,  print);
	multTestHelp("fffffff", "fffffff", "ffffffe0000001", &pass,  print);
	multTestHelp("111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
			"123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
			"136b06e70136b06e701679d16d191af4abf16d1a382d82d950feddbbaa99777755444431110eeddddccba84e708fe640c708fe8bfda6077e56826c68078ce438e39d186796c9cadb0eed30322127553969887a7dbb9e2", &pass,  print);
	multTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
			"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
			"1f08008cd191e290cb51e3e320f35c2b07b5dcd159e79f68bcf23c29d50324ce85a091ce86ecb91afd325d4488bf3ddff0ca418caf26d750d53783aa570a5cf47422df38fcf48eddecd44cebb63feef2e9d9e777f096021fd71424b5ce", &pass,  print);

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
	//smallMultTestHelp(268435455, "32", "31FFFFFCE", &pass,  print);
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
