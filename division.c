/*

 * division.c

 *
 *  Created on: Mar 29, 2020
 *      Author: Lien
 */

#include"division.h"

/*
INPUT : y, x
OUTPUT : quotient, r
with: x = y*quotient + r
BEWARE: x[0] > 1 and y[0] > 0 is both mandatory!!!
(the code is based on algorithm 14.20 in the handbook of applied cryptography)
*/
void division(WORD *quotient, WORD *r, WORD *x, WORD *y){
	WORD n = x[0] - 1;
	if (n<1){
		printf("Invalid input");
		return;
	}

	WORD t = y[0] - 1;
	WORD w;
	WORD i;
	WORD bmin1 = ~0;
	WORD xIsNeg = 0;
	WORD_3 div = 0;
	WORD_3 divCopy = 0;
	WORD ybiExp[SIZE] = {0};
	WORD res[SIZE] = {0};
	WORD a[SIZE] = {0};
	WORD b[SIZE] = {0};
	WORD yArray[SIZE] = {0};
	WORD qArray[SIZE] = {0};
	WORD ybExp[SIZE] ={0};

	WORD xCopy[SIZE] = {0};
	WORD yCopy[SIZE] = {0};
	copyWord(xCopy, x);
	copyWord(yCopy, y);

	//1.
	WORD q[SIZE] = {0};

	//2.

	shiftBase(ybExp, n-t, yCopy);	/*ybExp = y*b^(n-t)*/

	while (greaterThan(ybExp, xCopy) == 0 ){
		q[n-t] = q[n-t] + 1;
		subSelf(xCopy, ybExp);
	}

	//3.

	for(i=n; i>=t+1; i--){
		xIsNeg = 0;
		yCopy[0] = 0;
		xCopy[0] = 0;

		//3.1
		if(xCopy[i+1] == yCopy[t+1]){
			qArray[0] = 1;
			qArray[1] = bmin1;
			div = bmin1;
		}
		else{

			div = ((WORD_3) xCopy[i+1] << 2*BIT) + ((WORD_3) xCopy[i] << BIT) + (WORD_3) xCopy[i-1];  /*sum = x[i+1]*b+x[i-1+1]*/
			div = div/(((WORD_3) yCopy[t+1] << BIT) + (WORD_3) yCopy[t]) ;	/* q[i-t-1] = (x[i+1]*b+x[i-1+1])/y[t]*/

			qArray[1] = div;
			divCopy = div;
			qArray[2] = divCopy>>BIT;
			qArray[3] = divCopy>>2*BIT;

			w = 2;
			while (qArray[w] == 0  && w>0){
				w--;
			}
			qArray[0] = w;
		}

		//3.2
		yArray[1] = yCopy[t];
		yArray[2] = yCopy[t+1];
		w = 2;
		while (yArray[w] == 0  && w>0){
			w--;
		}
		yArray[0] = w;

		mult(a, qArray, yArray);   /* a = q[i-t-1]*(y[t+1]*b +y[t]) */

		b[1] = xCopy[i-1];
		b[2] = xCopy[i];
		b[3] = xCopy[i+1];	/* b = x[i+1]*b^2 + x[i]*b + x[i-1]*/
		w = 3;
		while (b[w] == 0  && w>0){
			w--;
		}
		b[0] = w;

		while (greaterThan(a,b)){
			div = div - 1;
			qArray[1] = div;
			divCopy = div;
			qArray[2] = divCopy>>BIT;
			w = 2;
			while (qArray[w] == 0  && w>0){
				w--;
			}
			qArray[0] = w;
			mult(a, qArray, yArray);

		}
		q[i-t-1] =  qArray[1];

		//3.3
		yCopy[0] = t + 1;
		w = n + 1;
		while (xCopy[w] == 0  && w>0){
			w--;
		}
		xCopy[0] = w;
		shiftBase(ybiExp, i-t-1, yCopy); 	/* ybiExp = y*b^(i-t-1)*/
		smallMult(res, q[i-t-1], ybiExp);   /*res = q[i-1-t+1]*y*b^(i-t-1)*/
		subUnsigned(xCopy, res, &xIsNeg, 0);

		//3.4
		if (xIsNeg == 1){

			WORD res[SIZE] = {0};
			sub(res, ybiExp, xCopy);
			copyWord(xCopy, res);
			q[i-t-1] = q[i-t-1] - 1;

		}
	}

	//4.
	//copying r right:
	copyWord(r,xCopy);

	//copying q right:
	for(i = 1; i<SIZE; i++){
	        quotient[i] = q[i-1];
	    }
	w = SIZE-1;
	while (quotient[w] == 0  && w>0){
		w--;
	}
	quotient[0] = w;


}


/*
calculates res = y * b^a, with b the base we are using (b = 2^BIT)
*/
void shiftBase(WORD *res, WORD a, WORD *y){
	WORD i;
	for(i=1; i<=y[0]; i++){
		res[i+a] = y[i];
	}
	res[0] = a + y[0];
	for(i=1; i<=a; i++){
		res[i] = 0;
	}
}


void divisionTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    divisionTestHelp("100000000", "100000000", "1", "0", &pass,  print);
    divisionTestHelp("200000000", "100000000", "2", "0", &pass,  print);
    divisionTestHelp("10000000000000000", "100000000", "100000000", "0", &pass,  print);
    divisionTestHelp("1548abf89", "23", "9BAD245","1A" , &pass,  print);
    divisionTestHelp("123456789abcdef", "1254786523", "FE3F3", "CACA8C4B6", &pass,  print);
    divisionTestHelp("12235489784561235485487541254ab154487564785c", "123548754876421235487", "FF0396F71A09B8066AF5A29", "5A3F12AFB64428BB78BD", &pass,  print);
    divisionTestHelp("ffff1254897456ab14efd5", "1254879a4587eabc", "df740a1", "189ddfee874f99", &pass,  print);
    divisionTestHelp("59832029383BCDBABCDBACAB", "39587BAFEEEEBDDC", "18f98bf45", "193e3c60983b5c5f", &pass,  print);
    divisionTestHelp("202aa479494d0fddf", "39587bafe", "8f98bf45", "219e81569", &pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1235487548764",
    		"2509f6005951d8fc3f787876c0ecd01f5088a58b2637701969a0dec603116913fdf301c6bad572e5fb7f36fb2f60f99d5c18c31bc", "2d8a3827a641", &pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1235",
    		"250A89678218E8BD18EF80AA2DE001610D428EB4838AB4219A960C42E35D91007528C5ADFC24D589058CAF5BF6F875E4578D3B5ED2CC7E42BE", "A5B", &pass,  print);
    divisionTestHelp("2A268D7B99FCAA3D7C205740536D537B1", "1",
		   "2A268D7B99FCAA3D7C205740536D537B1","0", &pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1",
		   "2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1","0", &pass,  print);
    divisionTestHelp("c8d459fef1e4912584587965214abcde4544edefbacfe87c0891", "d09d0a0a961bf86413b7e95f40ea3679", "F672AC4F81BD9805AF45", "18C4BAA0AAD71BDDB9BA318365C1A2F4", &pass,  print);
    divisionTestHelp("59832029383BCDBABCDBACABDACFFCAD2039587032945873209530958", "39587BAFEEEEBDDCC0293857023958BCDBCDBC723","18f98bf4570b07183", "3ebcad27cba71705f167feb8265f64e629e5af6f",&pass,  print);
    divisionTestHelp("abcdefabcdefabcdefabcdefabcdefa1264654865123154894652315489456132abcdededcecdcdceceddecf12345674894561231456",
			"124789aaaaabbbbbbbdddeeeeee6452131564684846a46b4654c6d46e456f4684654c65d4f456eefff",
			"966164f6e9a2bc174dac25ee764", "caa361930c488d0c172e94d88aef289fdc6743426da4a8cedb55668ffa1001f5fbb9512e5c99c3bba",&pass,  print);
    divisionTestHelp("1254789963332569898987894561321564847945613254897645123547894653215478465123548746513254587946513254678465132456786513245",
					"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccdddddddddddddd",
			"1b7eb4e614ccb81e","34343233f00bdcc00f2d4eb0c95313b063340124367835421026134117738b3a598465878bac5c30715efbff5de7df33ffebe5a5f", &pass,  print);
    divisionTestHelp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
			"100000000000000000000000000000000000000000000000000000000000000000000000000","ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", &pass,  print);
    divisionTestHelp("abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222222222222",
			 "abc1564687865412654896451548965423187465132864518465148651ac486d84651ce864514865132846513222222212222223", "1","fffffff", &pass,  print);
    divisionTestHelp("fffffff", "fffffff", "1", "0",&pass,  print);
    divisionTestHelp("123456789123456789151234564789456132564894aaaaabbbeeefdfefefdfffdfefffedfffdfefffefdfffde",
				"111111111111111111111111111111111111111111111111111111111111111111111111111111111111f",
			"11111","1233bcdf01233bfbcdf00f233f00bdd00f33f55555666999a8a9a9a8aaa8a9aaa98aaa8a9aaa9a7bcdcf", &pass,  print);
    divisionTestHelp("213a1513a51a6cda56cd1a648ad4ca615ad61ca6cd68d14c86dc4a86c48bef864bf8e61be531bf5648fb789bf4b5f61ebf3bfe486",
			"ef1564feb894feb891fe5b6f1eb65f154fe8f1e56e1bf48ad651cda65c1a65cda65da16d8c46da48d",
			"2393eec2cf2f8fbe9393be0a", "83674f6e74ba64242f6414ef251f584518261872881c821521a19a015b1f8ea19b73f792735fd104",&pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "123548764",
	    		"2509f5fe61c76365ce7f5b2e8c1255c703423907cffe5a87d56186cf7b3c9cb740fc49bb5103f57c37bede59b02667eb19741aa07df8e", "e0460239", &pass,  print);
    divisionTestHelp("2A12548745874587A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "2A12548745874587A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1",
	    		"1", "0", &pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "12354875ffff",
	    		"2509f5fee3f9ae6400d84e1ade185b67609da2d585263bee53116737c971d1129479f4cefe96d39177d26de9a64adda0bc13fa18b0", "c1869af5061", &pass,  print);
    TEST(pass)
    ENDTEST(print)
}

void divisionTestHelp(char xChar[], char yChar[], char expqChar[], char exprChar[], WORD* pass, WORD print){
    WORD x[SIZE] = {0};
    WORD y[SIZE] = {0};
    WORD q[SIZE] = {0};
    WORD r[SIZE] = {0};
    WORD expr[SIZE] = {0};
    WORD expq[SIZE] = {0};

    convert(x, xChar);
    convert(y, yChar);
    convert(expr, exprChar);
    convert(expq, expqChar);


    division(q, r, x, y);
    *pass &=( equalWord(r, expr) && equalWord(q, expq) );

    if(print){
        if(*pass ){
            printf("PASS \n");
        }
        else{
            printf("-- FAIL :/ \n");
        }
        printf("expected r : ");
        print_num(expr);
        printf("got      : ");
        print_num(r);

        printf("expected q : ");
        print_num(expq);
        printf("got      : ");
        print_num(q);
    }
}

