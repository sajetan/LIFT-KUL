/*

 * division.c

 *
 *  Created on: Mar 29, 2020
 *      Author: Lien
 */

#include"division.h"

//INPUT : y, x
//OUTPUT : quotient, r
//with: x = y*qutient + r
// BEWARE: x[0] > 1 and y[0] > 0 !!!
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
	WORD_2 div;
	WORD_2 divCopy;
	WORD ybiExp[SIZE] = {0};
	WORD res[SIZE] = {0};
	WORD a[SIZE] = {0};
	WORD b[SIZE] = {0};
	WORD yArray[SIZE] = {0};
	WORD qArray[SIZE] = {0};

	WORD ybExp[SIZE] ={0};

	//1.
	WORD q[SIZE] = {0};

	//2.

	shiftBase(ybExp, n-t, y);	//ybExp = y*b^(n-t)

	while (greaterThan(ybExp, x) == 0 ){
		q[n-t] = q[n-t] + 1;
		subSelf(x, ybExp);
	}

	//3.

	for(i=n; i>=t+1; i--){
		y[0] = 0;
		x[0] = 0;

		//3.1
		if(x[i+1] == y[t+1]){
			qArray[0] = 1;
			qArray[1] = bmin1;
			div = bmin1;
		}
		else{

			div = ((WORD_2) x[i+1] << BIT) + (WORD_2) x[i];  //sum = x[i+1]*b+x[i-1+1]
			div = div/y[t+1];	// q[i-t-1] = (x[i+1]*b+x[i-1+1])/y[t]

			qArray[1] = div;
			divCopy = div;
			qArray[2] = divCopy>>BIT;
			w = 2;
			while (qArray[w] == 0  && w>0){
				w--;
			}
			qArray[0] = w;
		}

		//3.2
		yArray[1] = y[t];
		yArray[2] = y[t+1];
		w = 2;
		while (yArray[w] == 0  && w>0){
			w--;
		}
		yArray[0] = w;

		mult(a, qArray, yArray);   // a = q[i-t-1]*(y[t+1]*b +y[t])

		b[1] = x[i-1];
		b[2] = x[i];
		b[3] = x[i+1];	// b = x[i+1]*b^2 + x[i]*b + x[i-1]
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
		y[0] = t + 1;
		x[0] = n + 1;
		shiftBase(ybiExp, i-t-1, y); 	// ybiExp = y*b^(i-t-1)
		smallMult(res, q[i-t-1], ybiExp);   //res = q[i-1-t+1]*y*b^(i-t-1)
		subUnsigned(x, res, &xIsNeg, 0);

		//3.4
		if (xIsNeg == 1){
			subSelf(x, ybiExp);
			q[i-t-1] = q[i-t-1] - 1;

		}
	}

	//4.
	//copying r right:
	copyWord(r,x);

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


//calculates res = y * b^a
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
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1235487548764",
    		"2509F6005951D8FC3F787876C0ECD01F5088A58B2637701969A0DEC603116913FDF301C6BAD572E5FB7F36FB2F60F99D5C18C31BC", "2D8A3827A641", &pass,  print);
    divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1235",
    		"250A89678218E8BD18EF80AA2DE001610D428EB4838AB4219A960C42E35D91007528C5ADFC24D589058CAF5BF6F875E4578D3B5ED2CC7E42BE", "A5B", &pass,  print);
   divisionTestHelp("2A268D7B99FCAA3D7C205740536D537B1", "1",
		   "2A268D7B99FCAA3D7C205740536D537B1","0", &pass,  print);
   divisionTestHelp("2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1", "1",
		   "2A268D7B993AF857AE700A19A753F791C066CD03C9EFC61A7D38DED3DD5A6910D551F5728BFCAA3D7C20A74C56799BA766E0E69F5740536D537B1","0", &pass,  print);
   divisionTestHelp("c8d459fef1e4912584587965214abcde4544edefbacfe87c0891", "d09d0a0a961bf86413b7e95f40ea3679", "F672AC4F81BD9805AF45", "18C4BAA0AAD71BDDB9BA318365C1A2F4", &pass,  print);
//    divisionTestHelp(32, 15, "8000000000000000000", &pass,  print);



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





void shiftBaseTest(WORD print){
    WORD pass = 1;

    BEGINTEST(print)

    shiftBaseTestHelp(2, "1C8","1c80000000000000000", &pass,  print);
    shiftBaseTestHelp(2,"123456789","1234567890000000000000000", &pass,  print);
//    shiftBaseTestHelp(3, "1000000000000000000000000", &pass,  print);
//    shiftBaseTestHelp(5, "10000000000000000000000000000000000000000", &pass,  print);
//    shiftBaseTestHelp(16, "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &pass,  print);
//    shiftBaseTestHelp(255, 2, "FE01", &pass,  print);
//    shiftBaseTestHelp(255, 1, "ff", &pass,  print);
//    shiftBaseTestHelp(32, 4, "100000", &pass,  print);
//    shiftBaseTestHelp(4, 15, "40000000", &pass,  print);
//    shiftBaseTestHelp(32, 15, "8000000000000000000", &pass,  print);



    TEST(pass)
    ENDTEST(print)
}

void shiftBaseTestHelp(WORD a, char yChar[], char expChar[], WORD* pass, WORD print){

	WORD y[SIZE] = {0};
    WORD res[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(y, yChar);

    convert(exp, expChar);

    shiftBase(res, a, y);
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
