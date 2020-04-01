/*
 * hash.c
 * 
 *  Created on: Mar 23, 2020
 *      Author: Ferdinand Hannequart
 */

#include"hash.h"


/* hash function (256 bit)
INPUTS: - in    : array of WORDS, length can be whatever
OUTPUT: - out   : array of WORDS, length = SIZE; will contain the 256 bit hash
*/
void hash256(WORD out[],  void* in){
    WORD numberBytes    = getNumberBytes(in);
    WORD len            = SIZE-1;
    WORD i              = 0;
    
    // 256 for 256 bit hash
    // SHA3_FLAGS_KECCAK is a constant used by sha3.c
    // input+1 instead of input to send the array without first element
    // numberBytes is the exact number of relevant bytes (without MSB zeros, so that H("65") = H("065"))
    // BIT*SIZE is the amount of bytes of array "out"

    in += BIT/8; // use pointer arithmetic to sent whole array except first element

    sha3_HashBuffer(256, SHA3_FLAGS_KECCAK, in, numberBytes, out, (BIT/8)*SIZE);
    for( i = SIZE; i>0; i--){ // shift all cells to free place for size
        out[i] = out[i-1];
    }
    while(out[len] == 0 && len > 0){
        len--;
    }
    out[0] = len;

}
/*  Returns the number of bytes of a number.
    All MSB zeros, i.e. all digits after the largest non-zero bit, are excluded.
    This function is important for the hash function, since the exact amount of bytes must be known.
    For example, 0xABC and 0x000000ABC have both 2 relevant bytes 
    and should therefore yield the same hash.*/
WORD getNumberBytes(WORD w[]){
    WORD len = w[0];
    WORD nZeros = 0;

    while(w[len] == 0 && len >0){ // in case the size was not updated correctly
        printf(  "\n\n WARNING FROM getNumberBytes():"
                "NUMBER OF ELEMENTS WAS NOT UPDATEd \n\n");
       len--; 
    }
    nZeros = getNumberZeroBytes(w[len]);
    return len*(BIT/8)-nZeros;
}

/*  Returns the amount of zero MSB bytes in one integer*/
WORD getNumberZeroBytes(WORD w){
    WORD nZeros = BIT/8;
    if(w == 0){
        return 0;
    }
    while(w != 0){
        w >>= 8;
        nZeros--;
    }
    return nZeros;
}

void hash256Test(WORD print){
    WORD pass = 1;
    //char pInit[] = "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e";
    //char p0[] = "4e03657aea45a94fc7d47ba826c8d667c0d1e6e33a64a036ec44f58fa12d6c45";
    //char p1[] = "9f0adad0a59b05d2e04a1373342b10b9eb16c57c164c8a3bfcbf46dccee39a21";
    //char p2[] = "d319cfe69680e37edd3a2a83a076b18f075bbfb86d2e915757c0cfe491b4bf9c";
    
    BEGINTEST(print)
    
    hash256TestHelp("abc", "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e",  &pass,  print);
    //hash256TestHelp("636261636261", p1,  &pass,  print);
    //hash256TestHelp("636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261", p2,  &pass,  print);

    TEST(pass)
    ENDTEST(print)
}
void hash256TestHelp(char inchar[], char expchar[], WORD* pass, WORD print){
    WORD hash[SIZE] = {0};
    WORD in[SIZE]   = {0};
    WORD exp[SIZE]  = {0};

    text2array(in, inchar);
    convert(exp, expchar);
    //print_num(in);
    hash256( hash, in);
    if(print){
        printf("result: ");
        print_num(hash);
        printf("expected: ");
        print_num(exp);
    }
    *pass &= equalWord(exp, hash);
}


void getNumberBytesTest(WORD print){
    WORD a[SIZE] = {0};
    WORD pass = 1;

    BEGINTEST(print)

    convert(a, "1");
    pass &= getNumberBytesTestHelp(1, a, print);

    convert(a, "64");
    pass &= getNumberBytesTestHelp(1, a, print);
    
    convert(a, "0000000000640000000000");
    pass &= getNumberBytesTestHelp(6, a, print);
    
    convert(a, "0008726353700");
    pass &= getNumberBytesTestHelp(5, a, print);

    convert(a, "000000000055000000000055000000000055");
    pass &= getNumberBytesTestHelp(13, a, print);
   
    convert(a, "00087977656000");
    pass &= getNumberBytesTestHelp(6, a, print);

    convert(a, "0008797765600BFCD0");
    pass &= getNumberBytesTestHelp(8, a, print);

    convert(a, "1000BC5D");
    pass &= getNumberBytesTestHelp(4, a, print);

    convert(a, "cdbf000c000cf");
    pass &= getNumberBytesTestHelp(7, a, print);
    
    convert(a, "cdbf000c000cfcdbf000c000cfcdbf000c000cfcdbf000c000cf");
    pass &= getNumberBytesTestHelp(26, a, print);

    convert(a, "cdbf000c000cfcdbf000c000cfcdbf000c000cfcdbf000c000cf876543212345");
    pass &= getNumberBytesTestHelp(32, a, print);
    
    TEST(pass)
    ENDTEST(print)

}

WORD getNumberBytesTestHelp(WORD exp, WORD w[], WORD print){
    WORD result = getNumberBytes(w);
    if (print){
        printf("expected %d, got %d  : ", exp, result);
        print_num(w);
    }
    return exp == result;
}
