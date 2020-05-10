/*
 * hash.c
 * 
 *  Created on: Mar 23, 2020
 *      Author: Ferdinand Hannequart
 */

#include"hash.h"


/* hash function (256 bit)
INPUTS: - in    : array of WORDS, length can be whatever. 
                Beware that the length field must be present and will be removed.
                By removing the length field, the input array is chnged as well, which is not ideal I know
OUTPUT: - out   : array of WORDS, length = SIZE; will contain the 256 bit hash
*/
void hash(WORD out[],  WORD* in, uint16_t sizeHash){
    assert (sizeHash == 256 ||sizeHash == 384 ||sizeHash == 612);

    WORD numberBytes    = getNumberBytes(in);
    WORD len            = sizeHash/BIT; // ne need to look further since only 256 bits
    WORD i              = 0;
    WORD copy[SIZE]     = {0};
    
    // 256 for 256 bit hash
    // SHA3_FLAGS_KECCAK is a constant used by sha3.c
    // input+1 instead of input to send the array without first element
    // numberBytes is the exact number of relevant bytes (without MSB zeros, so that H("65") = H("065"))
    // BIT*SIZE is the amount of bytes of array "out"

    for(i = 0; i<SIZE-1; i++){
        copy[i] = in[i+1];
    }
    //in += BIT/8; // use pointer arithmetic to send whole array except first element

    sha3_HashBuffer(sizeHash, SHA3_FLAGS_KECCAK, copy, numberBytes, out, (BIT/8)*SIZE);

    for( i = len; i>0; i--){ // shift all cells to free place for size
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



void hash256Test(WORD print){
    WORD pass = 1;
    //char pInit[] = "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e";
    //char p0[] = "4e03657aea45a94fc7d47ba826c8d667c0d1e6e33a64a036ec44f58fa12d6c45";
    //char p1[] = "9f0adad0a59b05d2e04a1373342b10b9eb16c57c164c8a3bfcbf46dccee39a21";
    //char p2[] = "d319cfe69680e37edd3a2a83a076b18f075bbfb86d2e915757c0cfe491b4bf9c";
    
    BEGINTEST(print)
    
    //hash256TestHelp("abc", "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e",  &pass,  print);
    hash256TestHelp("636261", "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e",  &pass,  print);
    //hash256TestHelp("636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261636261", p2,  &pass,  print);

    TEST(pass)
    ENDTEST(print)
}
void hash256TestHelp(char inchar[], char expchar[], WORD* pass, WORD print){
    WORD myHash[SIZE] = {0};
    WORD in[SIZE]   = {0};
    WORD exp[SIZE]  = {0};

    //text2array(in, inchar);
    convert(in, inchar);
    convert(exp, expchar);
    //print_num(in);
    hash( myHash, in, SIZEHASH);
    if(print){
        printf("input: ");
        print_num(in);
        printf("result: ");
        print_num(myHash);
        printf("expected: ");
        print_num(exp);
    }
    *pass &= equalWord(exp, myHash);
}

