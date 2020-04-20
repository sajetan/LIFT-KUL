/*
 * main.c
 */

#include"main.h"
#include"p256.h"
#include"chacha20_poly1305_interface.h"

int main(){
	
    /////// Tejas ///////
      //p256TestScalarMultJacobian(); //test scalar mult jacobian
      //p256TestScalarMultAffine();  //test scalar mult affine
      //p256TestAdditionAffine();  //test add affine
      //p256TestDoubleAffine();  //test double affine


      //padding_test();
      //test_poly_key();

	aead_test();

	//demoEntropyPool();	// demo entropy pool

    /////// Ferdinand ///////

  	
	addTest(0);
//	addSelfTest(0);
//	subTest(0);
//	subSelfTest(0);
 //	getNumberBytesTest(0);
//    shiftl1Test(0); 
//    shiftr1Test(0);
//	hash256Test(0);			// hash
//    inverseTest(0);			// inverse
	
//	randomTest();			// random numbers
	
	//demoEntropyPool();	// demo entropy pool

	
	//convertTest();



   /////// Lien ///////
// 	modTest(0);
// 	multTest(0);
// 	divisionTest(0);
// 	signatureTest(1);
}
