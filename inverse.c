/*
Ferdinand Hannequart
inverse.c
*/

#include"inverse.h"


/* This function computes the inverse z of a mod p, where
az mod m = 1, m is odd, a and m are coprime (i.e. gcd(a, m) = 1).

This function is essentially the binary extended gcd algorithm,
also called binary extended Euclidian algorithm.
The only difference is that since we are only looking for an inverse
and that we know that gcd(a, m) = 1, some unnecessary computations
are left out.

This algorithm has the adjective "binary" since it is a variant of 
the classical extended Euclidian algorithm: the computationnally
expensive divisions are replaced by elementary shifts and additions.
This implementation is more efficient than the classical approach 
when using multiple-precision numbers.

This implementation is based on Algorithm 14.61 
of the Handbook of Applied Cryptography.
As mentioned in note 14.64, the values A and C are not computed.

INPUT: two arrays a and m representing strictely positive integers, m odd, gcd(a,m) = 1.
OUTPUT: array number z, inverse of a mod m
*/
void inverse(WORD z[], WORD a[], WORD m[]){

    WORD u[SIZE] = {0};
    WORD v[SIZE] = {0};
    WORD B[SIZE] = {0};
    WORD D[SIZE] = {0};
    WORD BIsNeg = 0;    // will keep track of sign of B
    WORD DIsNeg = 0;    // will keep track of sign of D

    copyWord(u, m);     // u = m
    copyWord(v, a);     // v = a
    convert(B, "0");    // B = 0
    convert(D, "1");    // D = 1
    
    do{                                         // do
        while(~u[1] & 1){                       //while u is even, do the following
            shiftr1(u);                         // u = u >> 1
            if( ~B[1] & 1){                     // if B is even, then
                shiftr1(B);}                    // B = B >> 1
            else{                               // else
                subUnsigned(B, m, &BIsNeg, 0);  // B = B - m
                shiftr1(B);}}                   // B = B >> 1

        while(~v[1] & 1){                       //while v is even, do the following
            shiftr1(v);                         // v = v >> 1;
            if( ~D[1] & 1){                     // if D is even then
                shiftr1(D);}                    // D = D >> 1
            else{                               // else
                subUnsigned(D, m, &DIsNeg, 0);  // D = D - m
                shiftr1(D);}}                   // D = D >> 1

        if(geq(u, v)){// if u >= v
            subSelf(u, v);                      // u = u - v
            subUnsigned(B, D, &BIsNeg, DIsNeg);}// B = B - D
        else{                                   //else
            subSelf(v, u);                      // v = v - u
            subUnsigned(D, B, &DIsNeg, BIsNeg); // D = D - B
        }
    }while(!(u[0] == 0) && !(u[0]==1 && u[1]==0));  // while u different from 0
    
    // Different than in handbook of cryptography!
    // if D is negative, it must be brought to positive by adding m
    // while D is bigger in absolute values, subtract m
    // only after that, do abs(m)-abs(D)
    if(DIsNeg){
        while(greaterThan(D, m)){
            subSelf(D, m);  // D = D - m
        }            
        sub(z, m, D);
    }
    else{
        copyWord(z , D);                        // otherwise: z = D
    }

}

/* function that returns the absolute value of the subtraction
 and keeps track of the sign of each number.

 Essentially it yields a = |a-b| and indicates if a became negative or not
 through the aIsNeg pointer.

 This way, we never work with negative numbers.
 bIsNeg is not a pointer, since b's sign remains unchanged*/
void subUnsigned(WORD a[], WORD b[], WORD* aIsNeg, WORD bIsNeg){
    WORD result[SIZE] = {0};            // where the result will be stored temporarely

    // assert that 0 elements are positive
    if(  (a[0] == 0) || (a[0]==1 && a[1]==0)){
        assert(*aIsNeg == 0);
    }
    if(  (b[0] == 0) || (b[0]==1 && b[1]==0)){
        assert(bIsNeg == 0);
    }

    
    if (*aIsNeg ^ bIsNeg){              // case 1: opposite signs
        add(result, a, b);
    } 
    
    else if(!*aIsNeg && !bIsNeg){       // case 2: both positive
        if(greaterThan(b, a)){
            *aIsNeg = 1;
            sub(result, b, a);          // b-a;
        }
        else{
            sub(result, a, b);          // a-b;
        }

    } else if(*aIsNeg && bIsNeg){       // case 3: both negative
        if(greaterThan(a, b)){
            sub(result, a, b);          // a-b;
        }
        else{
            *aIsNeg = 0;
            sub(result, b, a);          // b-a;
        }
    }
    copyWord(a, result);                // copy result to pointer
}

void subUnsignedTest(WORD print){

    WORD pass = 1;

    //exp = |a-b|
    BEGINTEST(print)

    subUnsignedTestHelp("0", "0","0", 0, 0, 0, &pass, print);
    //subUnsignedTestHelp("0", "0","0", 1, 0, 0, &pass, print);
    //subUnsignedTestHelp("0", "0","0", 0, 1, 0, &pass, print);
    //subUnsignedTestHelp("0", "0","0", 1, 1, 0, &pass, print);
    
    subUnsignedTestHelp("1", "0","1", 0, 0, 0, &pass, print);
    //subUnsignedTestHelp("1", "0","1", 0, 1, 0, &pass, print);
    subUnsignedTestHelp("1", "0","1", 1, 0, 1, &pass, print);
    //subUnsignedTestHelp("1", "0","1", 1, 1, 1, &pass, print);
    
    subUnsignedTestHelp("0", "1","1", 0, 0, 1, &pass, print);
    //subUnsignedTestHelp("0", "1","1", 1, 0, 1, &pass, print);
    subUnsignedTestHelp("0", "1","1", 0, 1, 0, &pass, print);
    //subUnsignedTestHelp("0", "1","1", 1, 1, 0, &pass, print);
    
    subUnsignedTestHelp("1", "1","0", 0, 0, 0, &pass, print);
    subUnsignedTestHelp("1", "1","2", 1, 0, 1, &pass, print);
    subUnsignedTestHelp("1", "1","2", 0, 1, 0, &pass, print);
    subUnsignedTestHelp("1", "1","0", 1, 1, 1, &pass, print);
    /*
    
    subUnsignedTestHelp("fdc9e33b95883fb645040aaf2833ed3d3a08c19b384e5aa7d735f334fbc9ee451edf69add2d69ba58dc8f60176c71124b2daf56e990c536ac4db2fdbcf0fc223a18d59f9d0f4ab2995444efe3dc6fe04033ab7bcae4a4fc74292da454f911a0845f81d421d21324bbe6e8fc102f55b2cfd1d79b3fa1dc8b1a2c33d8c3ab377144031d6a6dccfecbef64c08df8341b212ad17b7a0c102a9e674f5e5f9e58c05a2ba491a83eea7856b0434e735b4f068cff724971ebfaf16f3ef837eda826df9d5d8deeddcbaeb9a4aa928ae548ee820c36ac6123ecd2687d73ab7826c3b6516cde55e607b6846b5bb608f57e8e8a955d1718478beef453cea1cff813f9044f437",
                        "bd7799c170bb189992a608b4876b9ed92fa9ac2b547ac67f0fe4083c921f6274be55239b141481c6ec16fa7ac27131c20c583c76a0cda80644a67d04b6d2b5c6485d7fe544e4b4c282ed561b48c743a9a00962d93cf81698c950412592f486b2f5d451a202328491cf8d442e243d3b5321fb52e5447f059eb2d53050471bace56d541ed425fb9cf7c9c98f2add7c8ab0ecdd700c54548362890de9f7b19b4b3295534723f5536f4c59c5287a9e1e947970f85bdb97ec3d790d0c2cf5b891bba7a5001ed666c0f67af2873ab2f6193651e92d5750a26b26b670a87ae5093bf4125a9699972baabe2100bcbd4f985e5fea53cf645dc809810c83d1ab105ffd4f3e",
                        "1bb417cfd0643584fd7aa1363af9f8c1669b26dc68cc92126e719fb718de950b9dd348d48e6eb1d6c79dff07c393842e6bf3331e539d9fb710981ace085e277e9e9ead9df15d95fec1831a519868e41ada3441a95eb4266600be31b6ae285a0bb3bcc6ee41f53b6dd8dfbd3ef273296801f18cc993e9cce5055986ddc81cf23f9ad85f57b02cb89b6c015980a60be3cc399f527ad15572d48fe03cff1972750d54f9c61a7e3faf4b75dfa0fb0530efd49681cf2fa579b546cfc8fabd03affb57d7ddf0cb321ac90c59bafe9078501571553f3698f6f91ae8dab5ffd5144a10ae03ff4fa1293f173dc614c15388107b5bbc553dd1cb74ebdf6a0d12c4ff0424375",
                        0, 1, 0, &pass, print);
    subUnsignedTestHelp("fdc9e33b95883fb645040aaf2833ed3d3a08c19b384e5aa7d735f334fbc9ee451edf69add2d69ba58dc8f60176c71124b2daf56e990c536ac4db2fdbcf0fc223a18d59f9d0f4ab2995444efe3dc6fe04033ab7bcae4a4fc74292da454f911a0845f81d421d21324bbe6e8fc102f55b2cfd1d79b3fa1dc8b1a2c33d8c3ab377144031d6a6dccfecbef64c08df8341b212ad17b7a0c102a9e674f5e5f9e58c05a2ba491a83eea7856b0434e735b4f068cff724971ebfaf16f3ef837eda826df9d5d8deeddcbaeb9a4aa928ae548ee820c36ac6123ecd2687d73ab7826c3b6516cde55e607b6846b5bb608f57e8e8a955d1718478beef453cea1cff813f9044f437",
                        "bd7799c170bb189992a608b4876b9ed92fa9ac2b547ac67f0fe4083c921f6274be55239b141481c6ec16fa7ac27131c20c583c76a0cda80644a67d04b6d2b5c6485d7fe544e4b4c282ed561b48c743a9a00962d93cf81698c950412592f486b2f5d451a202328491cf8d442e243d3b5321fb52e5447f059eb2d53050471bace56d541ed425fb9cf7c9c98f2add7c8ab0ecdd700c54548362890de9f7b19b4b3295534723f5536f4c59c5287a9e1e947970f85bdb97ec3d790d0c2cf5b891bba7a5001ed666c0f67af2873ab2f6193651e92d5750a26b26b670a87ae5093bf4125a9699972baabe2100bcbd4f985e5fea53cf645dc809810c83d1ab105ffd4f3e",
                        "1bb417cfd0643584fd7aa1363af9f8c1669b26dc68cc92126e719fb718de950b9dd348d48e6eb1d6c79dff07c393842e6bf3331e539d9fb710981ace085e277e9e9ead9df15d95fec1831a519868e41ada3441a95eb4266600be31b6ae285a0bb3bcc6ee41f53b6dd8dfbd3ef273296801f18cc993e9cce5055986ddc81cf23f9ad85f57b02cb89b6c015980a60be3cc399f527ad15572d48fe03cff1972750d54f9c61a7e3faf4b75dfa0fb0530efd49681cf2fa579b546cfc8fabd03affb57d7ddf0cb321ac90c59bafe9078501571553f3698f6f91ae8dab5ffd5144a10ae03ff4fa1293f173dc614c15388107b5bbc553dd1cb74ebdf6a0d12c4ff0424375",
                        1, 0, 1, &pass, print);
    subUnsignedTestHelp("fdc9e33b95883fb645040aaf2833ed3d3a08c19b384e5aa7d735f334fbc9ee451edf69add2d69ba58dc8f60176c71124b2daf56e990c536ac4db2fdbcf0fc223a18d59f9d0f4ab2995444efe3dc6fe04033ab7bcae4a4fc74292da454f911a0845f81d421d21324bbe6e8fc102f55b2cfd1d79b3fa1dc8b1a2c33d8c3ab377144031d6a6dccfecbef64c08df8341b212ad17b7a0c102a9e674f5e5f9e58c05a2ba491a83eea7856b0434e735b4f068cff724971ebfaf16f3ef837eda826df9d5d8deeddcbaeb9a4aa928ae548ee820c36ac6123ecd2687d73ab7826c3b6516cde55e607b6846b5bb608f57e8e8a955d1718478beef453cea1cff813f9044f437",
                        "bd7799c170bb189992a608b4876b9ed92fa9ac2b547ac67f0fe4083c921f6274be55239b141481c6ec16fa7ac27131c20c583c76a0cda80644a67d04b6d2b5c6485d7fe544e4b4c282ed561b48c743a9a00962d93cf81698c950412592f486b2f5d451a202328491cf8d442e243d3b5321fb52e5447f059eb2d53050471bace56d541ed425fb9cf7c9c98f2add7c8ab0ecdd700c54548362890de9f7b19b4b3295534723f5536f4c59c5287a9e1e947970f85bdb97ec3d790d0c2cf5b891bba7a5001ed666c0f67af2873ab2f6193651e92d5750a26b26b670a87ae5093bf4125a9699972baabe2100bcbd4f985e5fea53cf645dc809810c83d1ab105ffd4f3e",
                        "4052497a24cd271cb25e01faa0c84e640a5f156fe3d39428c751eaf869aa8bd0608a4612bec219dea1b1fb86b455df62a682b8f7f83eab648034b2d7183d0c5d592fda148c0ff6671256f8e2f4ffba5a633154e37152392e7942991fbc9c93555023cba01aeeadb9eee14b92deb81fd9db2226ceb59ec312efee0d3bf397ca2ed2ddb7d2b6d44fc72c8279b4a5c52761c03a47946cae2683ebe7fc0233f0ba7024f5d35ff954161eaa6fbebb16d1d456862c3b4327c2d97ae27751e4c9dc3e2e33decf06542aa3cfb6a173a198ceea718198baee2abb6120ca0f0787322922bb8ac7c6e43c9bf79a5fd29a99504af5e71db51461273bbbdd992dd62f3047a4f9",
                        0, 0, 0, &pass, print);
    subUnsignedTestHelp("fdc9e33b95883fb645040aaf2833ed3d3a08c19b384e5aa7d735f334fbc9ee451edf69add2d69ba58dc8f60176c71124b2daf56e990c536ac4db2fdbcf0fc223a18d59f9d0f4ab2995444efe3dc6fe04033ab7bcae4a4fc74292da454f911a0845f81d421d21324bbe6e8fc102f55b2cfd1d79b3fa1dc8b1a2c33d8c3ab377144031d6a6dccfecbef64c08df8341b212ad17b7a0c102a9e674f5e5f9e58c05a2ba491a83eea7856b0434e735b4f068cff724971ebfaf16f3ef837eda826df9d5d8deeddcbaeb9a4aa928ae548ee820c36ac6123ecd2687d73ab7826c3b6516cde55e607b6846b5bb608f57e8e8a955d1718478beef453cea1cff813f9044f437",
                        "bd7799c170bb189992a608b4876b9ed92fa9ac2b547ac67f0fe4083c921f6274be55239b141481c6ec16fa7ac27131c20c583c76a0cda80644a67d04b6d2b5c6485d7fe544e4b4c282ed561b48c743a9a00962d93cf81698c950412592f486b2f5d451a202328491cf8d442e243d3b5321fb52e5447f059eb2d53050471bace56d541ed425fb9cf7c9c98f2add7c8ab0ecdd700c54548362890de9f7b19b4b3295534723f5536f4c59c5287a9e1e947970f85bdb97ec3d790d0c2cf5b891bba7a5001ed666c0f67af2873ab2f6193651e92d5750a26b26b670a87ae5093bf4125a9699972baabe2100bcbd4f985e5fea53cf645dc809810c83d1ab105ffd4f3e",
                        "4052497a24cd271cb25e01faa0c84e640a5f156fe3d39428c751eaf869aa8bd0608a4612bec219dea1b1fb86b455df62a682b8f7f83eab648034b2d7183d0c5d592fda148c0ff6671256f8e2f4ffba5a633154e37152392e7942991fbc9c93555023cba01aeeadb9eee14b92deb81fd9db2226ceb59ec312efee0d3bf397ca2ed2ddb7d2b6d44fc72c8279b4a5c52761c03a47946cae2683ebe7fc0233f0ba7024f5d35ff954161eaa6fbebb16d1d456862c3b4327c2d97ae27751e4c9dc3e2e33decf06542aa3cfb6a173a198ceea718198baee2abb6120ca0f0787322922bb8ac7c6e43c9bf79a5fd29a99504af5e71db51461273bbbdd992dd62f3047a4f9",
                        1, 1, 1, &pass, print);

    subUnsignedTestHelp("d284c5052205ab7050a793722e5a9444aab52c50aef5b3bb2a58bb98ed40bbb8c708afd6bf414c98dcbfc6247ac90939595fead606e00e139a4469e30520839d0c8897ee1409597e34226a2c69aef528460c820bda289ff6dea21b653071c7322a28a949b57937a7376c457b31787ff46415205db863f8293a0ffe15b06df3a09a52c8cf9c3062e2a5a55a4290476578d079d7c57a0f41f12620168eb5983a30b1b5164087afb2e09556df7d6c503b11adb23200c10d8a1232915a3b60dc11d65f582f4800f3e2156e5fd0efffb44e900be3e20dfc76f24b5bb0f07a93fbf48ad4e458e986ba0ba2b4abc940d26dc2930a672a27d9d9c57b1ec2349dfa160ce2",
                        "8711b0a352bcd815c94b31de70bfafece8c368bc648b8e767b9c3e654eef8e83555af240b37b56a07cf190036fdb6e486d3ac23369ac4853ac25459ad859efda9348e7c7fa7364fa60f07d1f6d01e7777a81389f4589e04cb08bddf4b37c063319b6799b34e8a22f5e97f8b6a588b649f0a93a2a72e72a324b6c171d91c3992604257b8c73eebc007ba2a03663e0dcf95b6cba071bb282a86ee2864156cd109daab2939fe6158db149e8b8b1a46351024af2543a652b72140657fbbffebe06beeb57938edb82b062dc8604795820eef390729f10f614fbda930dc877acc329779dcee0b5bebf729d1f3b2ee10a157178ce01b667c883a6ef6361037258c4cb79",
                        "1599675a874c2838619f2c5509f1a44319378950d13814231a5f4f9fe3c304a3c1c63a21772bca33959b15627eaa47781c69aad09708c56674669af7ddd7a73779fd17fb60e7cbe789512e74bd6b0dc9fc08dbaab1fb280438f2df959e3edcd6543df22e4ea61d9d696043e31d701363e54be5a882b4b225b857c153342318cc69e78445c101f1ee32147fa78f42842722be691cc95c1c49995029cd00c654ace5c67a9e06dc54091df3f982f10b38c13f8a4863b2638fc2638e955fb5f9a18954aafc2d6dc7692784ae5d56957d53d839c56811ef28bee25eebeb8f240bf1e0272b3399f45797e3fd3e6f821dc83340bd868e08fa25d6c6a8223381052dad85b",
                        0, 1, 0, &pass, print);
    subUnsignedTestHelp("d284c5052205ab7050a793722e5a9444aab52c50aef5b3bb2a58bb98ed40bbb8c708afd6bf414c98dcbfc6247ac90939595fead606e00e139a4469e30520839d0c8897ee1409597e34226a2c69aef528460c820bda289ff6dea21b653071c7322a28a949b57937a7376c457b31787ff46415205db863f8293a0ffe15b06df3a09a52c8cf9c3062e2a5a55a4290476578d079d7c57a0f41f12620168eb5983a30b1b5164087afb2e09556df7d6c503b11adb23200c10d8a1232915a3b60dc11d65f582f4800f3e2156e5fd0efffb44e900be3e20dfc76f24b5bb0f07a93fbf48ad4e458e986ba0ba2b4abc940d26dc2930a672a27d9d9c57b1ec2349dfa160ce2",
                        "8711b0a352bcd815c94b31de70bfafece8c368bc648b8e767b9c3e654eef8e83555af240b37b56a07cf190036fdb6e486d3ac23369ac4853ac25459ad859efda9348e7c7fa7364fa60f07d1f6d01e7777a81389f4589e04cb08bddf4b37c063319b6799b34e8a22f5e97f8b6a588b649f0a93a2a72e72a324b6c171d91c3992604257b8c73eebc007ba2a03663e0dcf95b6cba071bb282a86ee2864156cd109daab2939fe6158db149e8b8b1a46351024af2543a652b72140657fbbffebe06beeb57938edb82b062dc8604795820eef390729f10f614fbda930dc877acc329779dcee0b5bebf729d1f3b2ee10a157178ce01b667c883a6ef6361037258c4cb79",
                        "1599675a874c2838619f2c5509f1a44319378950d13814231a5f4f9fe3c304a3c1c63a21772bca33959b15627eaa47781c69aad09708c56674669af7ddd7a73779fd17fb60e7cbe789512e74bd6b0dc9fc08dbaab1fb280438f2df959e3edcd6543df22e4ea61d9d696043e31d701363e54be5a882b4b225b857c153342318cc69e78445c101f1ee32147fa78f42842722be691cc95c1c49995029cd00c654ace5c67a9e06dc54091df3f982f10b38c13f8a4863b2638fc2638e955fb5f9a18954aafc2d6dc7692784ae5d56957d53d839c56811ef28bee25eebeb8f240bf1e0272b3399f45797e3fd3e6f821dc83340bd868e08fa25d6c6a8223381052dad85b",
                        1, 0, 1, &pass, print);
    subUnsignedTestHelp("d284c5052205ab7050a793722e5a9444aab52c50aef5b3bb2a58bb98ed40bbb8c708afd6bf414c98dcbfc6247ac90939595fead606e00e139a4469e30520839d0c8897ee1409597e34226a2c69aef528460c820bda289ff6dea21b653071c7322a28a949b57937a7376c457b31787ff46415205db863f8293a0ffe15b06df3a09a52c8cf9c3062e2a5a55a4290476578d079d7c57a0f41f12620168eb5983a30b1b5164087afb2e09556df7d6c503b11adb23200c10d8a1232915a3b60dc11d65f582f4800f3e2156e5fd0efffb44e900be3e20dfc76f24b5bb0f07a93fbf48ad4e458e986ba0ba2b4abc940d26dc2930a672a27d9d9c57b1ec2349dfa160ce2",
                        "8711b0a352bcd815c94b31de70bfafece8c368bc648b8e767b9c3e654eef8e83555af240b37b56a07cf190036fdb6e486d3ac23369ac4853ac25459ad859efda9348e7c7fa7364fa60f07d1f6d01e7777a81389f4589e04cb08bddf4b37c063319b6799b34e8a22f5e97f8b6a588b649f0a93a2a72e72a324b6c171d91c3992604257b8c73eebc007ba2a03663e0dcf95b6cba071bb282a86ee2864156cd109daab2939fe6158db149e8b8b1a46351024af2543a652b72140657fbbffebe06beeb57938edb82b062dc8604795820eef390729f10f614fbda930dc877acc329779dcee0b5bebf729d1f3b2ee10a157178ce01b667c883a6ef6361037258c4cb79",
                        "4b731461cf48d35a875c6193bd9ae457c1f1c3944a6a2544aebc7d339e512d3571adbd960bc5f5f85fce36210aed9af0ec2528a29d33c5bfee1f24482cc693c2793fb0261995f483d331ed0cfcad0db0cb8b496c949ebfaa2e163d707cf5c0ff10722fae80909577d8d44cc48befc9aa736be633457ccdf6eea3e6f81eaa5a7a962d4d432841a6e22a02ba0c2c66887f750d1dbe5e5cbf48b73d904d5ecb2993070282a0a19a252f4b6e26cbc7ecea0f62bfddc65be217fe2c395e7b621e0b1774009bb9257131b291d9cc76a7935f9c7b7142fd0661f670c8a32802e738cb1337157833c7fa990595709a5fc858511a3c6573c011561e8bbb61312ba1514169",
                        0, 0, 0, &pass, print);
    subUnsignedTestHelp("d284c5052205ab7050a793722e5a9444aab52c50aef5b3bb2a58bb98ed40bbb8c708afd6bf414c98dcbfc6247ac90939595fead606e00e139a4469e30520839d0c8897ee1409597e34226a2c69aef528460c820bda289ff6dea21b653071c7322a28a949b57937a7376c457b31787ff46415205db863f8293a0ffe15b06df3a09a52c8cf9c3062e2a5a55a4290476578d079d7c57a0f41f12620168eb5983a30b1b5164087afb2e09556df7d6c503b11adb23200c10d8a1232915a3b60dc11d65f582f4800f3e2156e5fd0efffb44e900be3e20dfc76f24b5bb0f07a93fbf48ad4e458e986ba0ba2b4abc940d26dc2930a672a27d9d9c57b1ec2349dfa160ce2",
                        "8711b0a352bcd815c94b31de70bfafece8c368bc648b8e767b9c3e654eef8e83555af240b37b56a07cf190036fdb6e486d3ac23369ac4853ac25459ad859efda9348e7c7fa7364fa60f07d1f6d01e7777a81389f4589e04cb08bddf4b37c063319b6799b34e8a22f5e97f8b6a588b649f0a93a2a72e72a324b6c171d91c3992604257b8c73eebc007ba2a03663e0dcf95b6cba071bb282a86ee2864156cd109daab2939fe6158db149e8b8b1a46351024af2543a652b72140657fbbffebe06beeb57938edb82b062dc8604795820eef390729f10f614fbda930dc877acc329779dcee0b5bebf729d1f3b2ee10a157178ce01b667c883a6ef6361037258c4cb79",
                        "4b731461cf48d35a875c6193bd9ae457c1f1c3944a6a2544aebc7d339e512d3571adbd960bc5f5f85fce36210aed9af0ec2528a29d33c5bfee1f24482cc693c2793fb0261995f483d331ed0cfcad0db0cb8b496c949ebfaa2e163d707cf5c0ff10722fae80909577d8d44cc48befc9aa736be633457ccdf6eea3e6f81eaa5a7a962d4d432841a6e22a02ba0c2c66887f750d1dbe5e5cbf48b73d904d5ecb2993070282a0a19a252f4b6e26cbc7ecea0f62bfddc65be217fe2c395e7b621e0b1774009bb9257131b291d9cc76a7935f9c7b7142fd0661f670c8a32802e738cb1337157833c7fa990595709a5fc858511a3c6573c011561e8bbb61312ba1514169",
                        1, 1, 1, &pass, print);
    
    subUnsignedTestHelp("22d8cf51476b7d01ec2b8ecff0881b52be290848e5f9822b1bcdc564bdb32683bc2673c87f1ba7df718b03cb8da3d069032a7fb324198022c8ba1fe7f10403f1099a8937a21fae7746e53f39a5f9c9de951b17d329beb28d2d306030478086d768a6c84fe43a29046dc4774bf9ace58949433420d52da354b8fb2be67cb3063f5a31df9f9cffad54dfc8ce8c2b51bd46616e2f6a6e9bd87bef3fa9568017b8ad92084343af56c586d1d7f3b1e0489ffc07ba354eb8f1e53aa6eeb8718dbff9ca80730774a5100e9a7a1c416639d985d5c1ae61fb5a8f74bfda61bfd8750cf53606b367dd1f90385847583c1e56b59100edfecc8a1f26026947f2ba9ece88ef07",
                        "787bfc1e96053c626946bfeca78f5273947455999f068a7b0c65f5674cad2dcd344c3c380097f10db6da7ce2453019809605af3aa6ea4f9404db5376be457181732e60e4caaa76826bd275db3be2852b0869b43eed6ecc9ab566ca299130fa21e5ae1a4536bf4ce9f761860792f0feed769876c8de84c2d4ff9019186d491afaf17b3a4021da272e11a48930f9d2cb1929eecae82e20c50ba7c0d858858932f88539ac957c4b4e39e4320c7a0517f7a64bb7fe6df29cf0964039ca663399c84677056e576db55a8dae4f1848aabdefe0f4659fe46b8129bf44f866c0cb71f34d5fc145bb88b76dcd7d37d7c6f169edee7621e6f09f22884c5c93a3be9aa3b605",
                        "9b54cb6fdd70b96455724ebc98176dc6529d5de285000ca62833bacc0a605450f072b0007fb398ed286580add2d3e9e999302eedcb03cfb6cd95735eaf4975727cc8ea1c6cca24f9b2b7b514e1dc4f099d84cc12172d7f27e2972a59d8b180f94e54e2951af975ee6525fd538c9de476bfdbaae9b3b26629b88b44fee9fc213a4bad19dfbed9d482f16d57bd2524885f8b5cfa529cbc9d87970081af05a0eba61741efd92ba213c0b60a002be56097a2537233bcab8ed5d0e72882d7c159c210f77875cc12c56928286b59aee49775b6b61401dfc6109e7f1f5a2699407ee8836674ad98a847a625c49013e5481f7eef6420b37abe488ab5a4865e5d692ca50c",
                        0, 1, 0, &pass, print);
    subUnsignedTestHelp("22d8cf51476b7d01ec2b8ecff0881b52be290848e5f9822b1bcdc564bdb32683bc2673c87f1ba7df718b03cb8da3d069032a7fb324198022c8ba1fe7f10403f1099a8937a21fae7746e53f39a5f9c9de951b17d329beb28d2d306030478086d768a6c84fe43a29046dc4774bf9ace58949433420d52da354b8fb2be67cb3063f5a31df9f9cffad54dfc8ce8c2b51bd46616e2f6a6e9bd87bef3fa9568017b8ad92084343af56c586d1d7f3b1e0489ffc07ba354eb8f1e53aa6eeb8718dbff9ca80730774a5100e9a7a1c416639d985d5c1ae61fb5a8f74bfda61bfd8750cf53606b367dd1f90385847583c1e56b59100edfecc8a1f26026947f2ba9ece88ef07",
                        "787bfc1e96053c626946bfeca78f5273947455999f068a7b0c65f5674cad2dcd344c3c380097f10db6da7ce2453019809605af3aa6ea4f9404db5376be457181732e60e4caaa76826bd275db3be2852b0869b43eed6ecc9ab566ca299130fa21e5ae1a4536bf4ce9f761860792f0feed769876c8de84c2d4ff9019186d491afaf17b3a4021da272e11a48930f9d2cb1929eecae82e20c50ba7c0d858858932f88539ac957c4b4e39e4320c7a0517f7a64bb7fe6df29cf0964039ca663399c84677056e576db55a8dae4f1848aabdefe0f4659fe46b8129bf44f866c0cb71f34d5fc145bb88b76dcd7d37d7c6f169edee7621e6f09f22884c5c93a3be9aa3b605",
                        "9b54cb6fdd70b96455724ebc98176dc6529d5de285000ca62833bacc0a605450f072b0007fb398ed286580add2d3e9e999302eedcb03cfb6cd95735eaf4975727cc8ea1c6cca24f9b2b7b514e1dc4f099d84cc12172d7f27e2972a59d8b180f94e54e2951af975ee6525fd538c9de476bfdbaae9b3b26629b88b44fee9fc213a4bad19dfbed9d482f16d57bd2524885f8b5cfa529cbc9d87970081af05a0eba61741efd92ba213c0b60a002be56097a2537233bcab8ed5d0e72882d7c159c210f77875cc12c56928286b59aee49775b6b61401dfc6109e7f1f5a2699407ee8836674ad98a847a625c49013e5481f7eef6420b37abe488ab5a4865e5d692ca50c",
                        1, 0, 1, &pass, print);
    subUnsignedTestHelp("22d8cf51476b7d01ec2b8ecff0881b52be290848e5f9822b1bcdc564bdb32683bc2673c87f1ba7df718b03cb8da3d069032a7fb324198022c8ba1fe7f10403f1099a8937a21fae7746e53f39a5f9c9de951b17d329beb28d2d306030478086d768a6c84fe43a29046dc4774bf9ace58949433420d52da354b8fb2be67cb3063f5a31df9f9cffad54dfc8ce8c2b51bd46616e2f6a6e9bd87bef3fa9568017b8ad92084343af56c586d1d7f3b1e0489ffc07ba354eb8f1e53aa6eeb8718dbff9ca80730774a5100e9a7a1c416639d985d5c1ae61fb5a8f74bfda61bfd8750cf53606b367dd1f90385847583c1e56b59100edfecc8a1f26026947f2ba9ece88ef07",
                        "787bfc1e96053c626946bfeca78f5273947455999f068a7b0c65f5674cad2dcd344c3c380097f10db6da7ce2453019809605af3aa6ea4f9404db5376be457181732e60e4caaa76826bd275db3be2852b0869b43eed6ecc9ab566ca299130fa21e5ae1a4536bf4ce9f761860792f0feed769876c8de84c2d4ff9019186d491afaf17b3a4021da272e11a48930f9d2cb1929eecae82e20c50ba7c0d858858932f88539ac957c4b4e39e4320c7a0517f7a64bb7fe6df29cf0964039ca663399c84677056e576db55a8dae4f1848aabdefe0f4659fe46b8129bf44f866c0cb71f34d5fc145bb88b76dcd7d37d7c6f169edee7621e6f09f22884c5c93a3be9aa3b605",
                        "55a32ccd4e99bf607d1b311cb7073720d64b4d50b90d084ff09830028efa07497825c86f817c492e454f7916b78c491792db2f8782d0cf713c21338ecd416d906993d7ad288ac80b24ed36a195e8bb4c734e9c6bc3b01a0d883669f949b0734a7d0751f5528523e5899d0ebb994419642d5542a809571f804694ed31f09614bb97495aa084da79d931dbbaa4ce810dd2c8809b7dbf84ec8fb8812f0205717a4af3316951ccf488b3125a18c824cf57aa43fdc91f39ab0b5b994b11f4a5d9ce7bf69266e2c8a54bf33432d6e270e46a0b32b73de910f1b4ff6a96a6e85664fe17590dddde6927357535df9ba89ab45ced88231a667ffc85e314a0e91fcc1ac6fe",
                        0, 0, 1, &pass, print);
    subUnsignedTestHelp("22d8cf51476b7d01ec2b8ecff0881b52be290848e5f9822b1bcdc564bdb32683bc2673c87f1ba7df718b03cb8da3d069032a7fb324198022c8ba1fe7f10403f1099a8937a21fae7746e53f39a5f9c9de951b17d329beb28d2d306030478086d768a6c84fe43a29046dc4774bf9ace58949433420d52da354b8fb2be67cb3063f5a31df9f9cffad54dfc8ce8c2b51bd46616e2f6a6e9bd87bef3fa9568017b8ad92084343af56c586d1d7f3b1e0489ffc07ba354eb8f1e53aa6eeb8718dbff9ca80730774a5100e9a7a1c416639d985d5c1ae61fb5a8f74bfda61bfd8750cf53606b367dd1f90385847583c1e56b59100edfecc8a1f26026947f2ba9ece88ef07",
                        "787bfc1e96053c626946bfeca78f5273947455999f068a7b0c65f5674cad2dcd344c3c380097f10db6da7ce2453019809605af3aa6ea4f9404db5376be457181732e60e4caaa76826bd275db3be2852b0869b43eed6ecc9ab566ca299130fa21e5ae1a4536bf4ce9f761860792f0feed769876c8de84c2d4ff9019186d491afaf17b3a4021da272e11a48930f9d2cb1929eecae82e20c50ba7c0d858858932f88539ac957c4b4e39e4320c7a0517f7a64bb7fe6df29cf0964039ca663399c84677056e576db55a8dae4f1848aabdefe0f4659fe46b8129bf44f866c0cb71f34d5fc145bb88b76dcd7d37d7c6f169edee7621e6f09f22884c5c93a3be9aa3b605",
                        "55a32ccd4e99bf607d1b311cb7073720d64b4d50b90d084ff09830028efa07497825c86f817c492e454f7916b78c491792db2f8782d0cf713c21338ecd416d906993d7ad288ac80b24ed36a195e8bb4c734e9c6bc3b01a0d883669f949b0734a7d0751f5528523e5899d0ebb994419642d5542a809571f804694ed31f09614bb97495aa084da79d931dbbaa4ce810dd2c8809b7dbf84ec8fb8812f0205717a4af3316951ccf488b3125a18c824cf57aa43fdc91f39ab0b5b994b11f4a5d9ce7bf69266e2c8a54bf33432d6e270e46a0b32b73de910f1b4ff6a96a6e85664fe17590dddde6927357535df9ba89ab45ced88231a667ffc85e314a0e91fcc1ac6fe",
                        1, 1, 1, &pass, print);
                        */

    TEST(pass)
    ENDTEST(print)
}
void subUnsignedTestHelp(char aChar[], char bChar[], char expChar[], WORD aIsNeg, WORD bIsNeg, WORD expIsNeg, WORD* pass, WORD print){
    WORD a[SIZE] = {0};
    WORD b[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(a, aChar);
    convert(b, bChar);
    convert(exp, expChar);

    subUnsigned(a, b, &aIsNeg, bIsNeg);
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

/* Test function for the inverse */
void inverseTest(WORD print){
    BEGINTEST(print)
    WORD pass = 1;
    inverseTestHelp("5", "3", "2", print, &pass);
    
    inverseTestHelp("10f", "17f", "6a", print, &pass);
    inverseTestHelp("3d666e7", "3c401d", "2f6eed", print, &pass);
    inverseTestHelp("1", "1", "1", print, &pass);
    inverseTestHelp("1", "5", "1", print, &pass);
    inverseTestHelp("39", "109a87e1", "f25ad5d", print, &pass);
    inverseTestHelp("109a87e1", "39", "5", print, &pass);
    inverseTestHelp("36", "2233", "e91", print, &pass);
    inverseTestHelp("253af5dfef54b657c4", 
                    "d390009f057907971c18dbcb", 
                    "56fdc5ec0705d17ff0c36db6", print, &pass);
                    
    // erreur:
///*
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                    "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                    "b265417f55a4d15b2cd11b5f5c6a31d3eed0383d8a98e5d88bbe6947803d3598", print, &pass);
  //                  */
    
    inverseTestHelp("eac28bdfc52014e49be02d07b79274e09db32078f92bfcf2a59c1109c4a0ae63", 
                "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "be4360e401a050e4aa36881e5e76c58ce028e3f1b4473458f624ef49c0f45a23", print, &pass); 
                

    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "ffffffff10000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "d4f191fe4049464a8df554f9741bc957719d95c2ff8b305956758f0dbdeec833", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "ffffffff30000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "a44265fd2b29919777fbf7c7de4c02f7eaac2b61682eb4ab34f25c6893cc3ec0", print, &pass);




    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "9fffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "730a9c3f630a9330b149888752e07b595cc9b72e02209145a9f4aede782cffa9", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "0fffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "ce5073f9974c2ad2408bda8240816664a6c3976ce29bedb4d6d26d24666c1e6", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "fffffffd00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "eb7d57cec0ed1cdda22bbdca1b46ee3c8fa474f067b76ce90065f8e3d870a7a5", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "dfffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "1443f6a0cc14a4a9260bb5be0685632d4206e83b98ad6cbbe45f105fd9e57aaf", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "ffffffff000000009fffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "dd8c93e6fee772085ac319e135b98f325ba2561ab435c293e824eea9c0d8cdee", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "fffffffff00000000fffffffffffffffbce6faada7179e84f3b9cac2fc632551",
                "3926e1a16a3abd2ca7f4d41a2f52c2a5a1242df8cb26d2c40774d9d017318f3f", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac3fc632551",
                "728bede49761420232d2add470fa5170523c92939baef7942c00839fd1cca0fe", print, &pass);
    inverseTestHelp("eac28bdfc52014e49be02d07b79273e09db32078f92bfcf2a59c1109c4a0ae61", 
                "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc732551",
                "39dd30805ba96ce0b9064252f00c3a5b84afb4927d947a6eebfa1346161fd046", print, &pass);
/*
        inverseTestHelp("5", "3", "2", print, &pass);
    inverseTestHelp("271", "383", "106", print, &pass);
    inverseTestHelp("64382695", "3948573", "3108589", print, &pass);
    inverseTestHelp("1", "1", "1", print, &pass);
    inverseTestHelp("1", "5", "1", print, &pass);
    inverseTestHelp("57", "278562785", "254127453", print, &pass);
    inverseTestHelp("278562785", "57", "5", print, &pass);
    inverseTestHelp("54", "8755", "3729", print, &pass);
    */
    TEST(pass)
    ENDTEST(print)
}

void inverseTestHelp(char aChar[], char mChar[], char expChar[], WORD print, WORD* pass){
    WORD a[SIZE] = {0};
    WORD m[SIZE] = {0};
    WORD exp[SIZE] = {0};
    WORD z[SIZE]={0}; // where the result will be stored

    convert(a, aChar);
    convert(m, mChar);
    convert(exp, expChar);

    inverse(z, a, m);
    *pass &= equalWord( exp,z);
       
    if(print){
        if(equalWord( exp,z) ){
            printf("PASS \n");
        }
        else{
            printf("-- FAIL :/ \n");
        }
        printf("expected : ");
        print_num(exp);
        printf("got      : ");
        print_num(z);
    }
}

/*
begcd.c
Ferdinand Hannequart
purpose: computing the gcd using the binary gcd algorithm. 
This is a recursive implementation
#include"begcd.h"
This function implements the binary extended gcd algorithm,
also called binary extended Euclidian algorithm.
In addition to computing gcd(x, y), 
it also computes the coefficients a and b of the Bezout's equality:
ax + by = gcd(a, b).
This algorithm has the adjective "binary" since it is a variant of 
the classical extended Euclidian algorithm: the computationnally
expensive divisions are replaced by elementary shifts and additions.
This implementation is more efficient than the classical approach 
when using multiple-precision numbers.
This implementation is based on Algorithm 14.61 
of the Handbook of Applied Cryptography
INPUT: two strictely positive integers x and y
OUTPUT: integers a, b and v = gcd(x, y)
void begcd(WORD* a, WORD* b, WORD* r, WORD x, WORD y){
    if(x <= 0 || y <= 0){
        printf("INVALID INPUT: input must be strictly positive");
    }
    WORD g = 1;
    while(~x & 1 && ~y & 1) //while x and y are both even
    {
        x = x >> 1;
        y = y >> 1;
        g = g << 1;
    }
    WORD u = x, v = y, A = 1, B = 0, C = 0,  D = 1;
    do{
        while(~u & 1) //while u is even, do the following
        {
            u = u >> 1;
            if(~A & 1 && ~B & 1) // if both A and B are even, then
            {
                A = A >> 1;
                B = B >> 1;
            }
            else
            {
                A = (A+y) >> 1;
                B = (B-x) >> 1;
            }
        }
        while(~v & 1) //while v is even, do the following
        {
            v = v >> 1;
            if(~C & 1 && ~D & 1) // if both C and D are even, then
            {
                C = C >> 1;
                D = D >> 1;
            }
            else
            {
                C = (C+y) >> 1;
                D = (D-x) >> 1;
            }        
        }
        // check which number is larger and subtract
        if(u>=v){
            u = u - v;
            A = A - C;
            B = B - D;
        }
        else{
            v = v - u;
            C = C - A;
            D = D - B;
        }
    }while(u != 0);
    // end, values are placed in the pointers:
    *a = C;
    *b = D;
    *r = (g*v);
}
Test function for the above begcd 
void begcdTest(){
    // template:
    // begcdTestHelp( x,  y, aExp, bExp, rExp)
    begcdTestHelp(1, 1, 0, 1, 1); 
    begcdTestHelp(693, 609, -181, 206, 21); //Handbook of Applied Crypto
    begcdTestHelp(609, 693, 173, -152, 21); 
    begcdTestHelp(240, 46, 14, -73, 2);    //Wiki
    begcdTestHelp(21474836, 10737418, 0, 1, 10737418); 
    begcdTestHelp(21474836, 10737416, 1, -2, 4); 
}
 Auxiliary test function for prints.
INPUT:  - x , y are the inputs of binary extended GCD algorithm
        - aExp, bExp are the expected coefficients
        - rExp is the expected gcd
OUTPUT: void
    
    WORD a=0, b=0, r=0; // where the result will be stored
    void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp){
begcd(&a, &b, &r, x, y);
    if( a == aExp  && b == bExp  && r == rExp ){
        printf("%s - PASS\n", __FILE__);
    }
    else{
        printf("%s - FAIL: expected a=%d, b=%d, gcd=%d but got a=%d, b=%d, gcd=%d\n", __FILE__, aExp, bExp, rExp, a, b, r);
    } 
}
void verifyCoefficients(WORD x, WORD y, WORD expGCD ){
    WORD a=0, b=0, r=0; // where the result will be stored
    begcd(&a, &b, &r, x, y);
    if(expGCD == r && expGCD == (a*x + b*y)){
        printf("%s - x=%d, y=%d, a=%d, b=%d, gcd=%d is a valid combination ", __FILE__, x, y, a, b, expGCD);
    }
    else{
        printf("%s - FAIL: x=%d, y=%d, a=%d, b=%d, gcd=%d is not valid", __FILE__, x, y, a, b, expGCD);
    }
}
///////////////////////////////
Ferdinand Hannequart
begcd.h
purpose: contains the prototypes, defines and includes
#ifndef DEF_BEGCD
#define DEF_BEGCD
#include"main.h"
void begcd(WORD* a, WORD* b, WORD* r, WORD x, WORD y);
void begcdTest();
void begcdTestHelp(WORD x, WORD y, WORD aExp, WORD bExp, WORD rExp);
void verifyCoefficients(WORD x, WORD y, WORD expGCD );
#endif
*/






/* ////////// GCD ////////////////

Ferdinand Hannequart
gcd.c

purpose: computing the gcd using the binary gcd algorithm. 
This is a recursive implementation based on the one of the wikipedia page.

#include"gcd.h"

WORD gcd(WORD u, WORD v){

    // if inputs are initially both 0, default output is 0
    // otherwise the program will ALWAYS end here
    if(u == v){
        return u;
    }

    // happens only if v is initially 0??
    if(u == 0){
        return v;
    }

    // happens only if v is initially 0??
    if(v == 0){
        return u;
    }

    if (~u & 1){            // u even
        if (~v & 1){        // u even, v even
            return gcd(u>>1, v>>1)<<1;
        }
        else {               //u even, v odd
            return gcd(u>>1, v);
        }
    }
    else{
        if (~v & 1){        // u odd, v even
        return gcd(u, v>>1);
        }
    }

    // both are odd, we must find which one is greater and subtract
    if (u>v){
        return gcd ((u-v)>>1, v);
    }
    else{
        return gcd ((v-u)>>1, u);
    }
}

void gcdTest(WORD print)
{
    BEGINTEST(print)
    WORD pass = 1;
    // corner cases
    gcdTestPrint(0, 0, 0, print, &pass);
    gcdTestPrint(1, 0, 1, print, &pass);
    gcdTestPrint(2, 0, 2, print, &pass);
    gcdTestPrint(0, 1, 1, print, &pass);
    gcdTestPrint(0, 2, 2, print, &pass);
    gcdTestPrint(249856,249856 , 249856, print, &pass);

    // other cases
    gcdTestPrint(23, 17, 1, print, &pass);
    gcdTestPrint(20, 36, 4, print, &pass);
    gcdTestPrint(60, 36, 12, print, &pass);
    gcdTestPrint(123947628, 563487258, 6, print, &pass);
    gcdTestPrint(568790880, 58, 2, print, &pass);
    TEST(pass)
    ENDTEST(print)
}

void gcdTestPrint(WORD u, WORD v, WORD exp, WORD print, WORD* pass){
    *pass &= exp == gcd(u, v);
    if(print){
        if(*pass){
            printf("Test OK: The gcd of %d and %d is %d\n", u, v, exp);
        } else{
            printf("FAIL: The gcd of %d and %d is %d, expected %d\n", u, v, gcd(u, v), exp);
        }
    }
}



Ferdinand Hannequart
gcd.h

purpose: contains the prototypes, defines and includes


#ifndef DEF_GCD
#define DEF_GCD

#include"main.h"

WORD gcd(WORD u, WORD v);
void gcdTest(WORD print);
void gcdTestPrint(WORD u, WORD v, WORD exp, WORD print, WORD* pass);

#endif
*/