/*
 * hash.c
 * 
 *  Created on: Mar 23, 2020
 *      Author: Ferdinand Hannequart
 */

#include"hash.h"


/* hash function (256 bit)
INPUTS: - input  : array of length smaller than 2^BIT
        - hash   : array of length SIZE, the hash value will be stored there
*/
void hash256(WORD hash[], WORD input[]){
    WORD bytes = getNumberBytes(input);
    input = input+1; // don't take the first element which is the size
    sha3_HashBuffer(256, SHA3_FLAGS_KECCAK, input, bytes, hash, BIT*SIZE);
    for(WORD i = SIZE-1; i>0; i--){ // shift all cells to free place for size
        hash[i] = hash[i-1];
    }
    WORD len = SIZE-1;
    while(hash[len] == 0){
        len--;
    }
    hash[0] = len;
}
/*  Returns the number of bytes of a number.
    All MSB zeros, i.e. all digits after the largest non-zero bit, are excluded.
    This function is important for the hash, since the exact amount of bytes must be known.
    For example, 0xA6 and 0x00A6 have both 1 relevant byte 
    and should therefore yield the same hash.*/
WORD getNumberBytes(WORD w[]){
    WORD len = w[0];
    while(w[len] == 0){ // in case the size was not updated correctly
        len--; 
    }
    WORD nZeros = getNumberZeroBytes(w[len]);
    return len*(BIT/8)-nZeros;
}

/*  Returns the amount of zero MSB bytes in one integer*/
WORD getNumberZeroBytes(WORD w){
    WORD nZeros = BIT/8;
    while(w != 0){
        w >>= 8;
        nZeros--;
    }
    return nZeros;
}

void hash256Test(WORD print){
    WORD hash[SIZE] = {0};
    WORD in[SIZE] = {0};
    WORD exp[SIZE] = {0};

    convert(in, "636261");
    convert(exp, "456c2da18ff544ec36a0643ae3e6d1c067d6c826a87bd4c74fa945ea7a65034e");
    hash256( hash, in);
    if(print){
        printf("\n---- %s -----\n", __func__);
        printf("result: ");
        print_num(hash);
        printf("expected: ");
        print_num(exp);
        if(equalWord(exp, hash)){
            printf("OK, all tests passed for %s\n", __func__);
        }
        else{
            printf("FAIL :/ - %s\n", __func__);
        }
        printf("---------end %s-------------\n\n", __func__);
    } else{
        if(equalWord(exp, hash)){
            printf("OK, all tests passed for %s\n", __func__);
        }
        else{
            printf("FAIL :/ - %s\n", __func__);
        }
    }
}
void getNumberBytesTest(WORD print){
    WORD a[SIZE] = {0};
    WORD exp = 0;
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

    TEST(pass)
    ENDTEST(print)

}

WORD getNumberBytesTestHelp(WORD exp, WORD w[], WORD print){
    WORD result = getNumberBytes(w);
    if (print){
        print_num(w);
        printf("expected %d, got %d\n", exp, result);
    }
    return exp == result;
}
