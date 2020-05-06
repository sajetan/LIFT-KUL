/*
 * utilities.c
 * 
 * 
 */

#include"utilities.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Converts a string of a hexadecimal numbers into an array of words.
 * out[0] contains the length
 * out[1] is the lsb
 * out[out[0]] is the msb
 * example: suppose that WORD is uint8_t:
 * in: "8F59B"
 * out: [3, 9B, F5, 8]
 * !!! out[] must have size SIZE, and therefore the string must be shorter that BIT/4*SIZE-1
 */
void convert(WORD *out, const char *in){
	WORD len = strlen(in);
	if(len >= BIT/4*SIZE-1){
		printf("!!!!! max size exceeded !!!!\n\n\n");
	}

	for(WORD i = 0; i<SIZE; i++){ // set array to zero
		out[i] = 0;
	}

	WORD stopEarly = 0; // handle MSB zeros and remove them
	while( stopEarly<len && in[stopEarly] == '0'){
		stopEarly++;
	}
	WORD index = 0; 	// keeps track of the cells of the array
	WORD current = 0;	// will store the number as a char
	WORD t = 0;			// will store the number as an integer
	for(WORD i = 0; i<len-stopEarly ; i++){
		index = i/(BIT/4)+1;
		current = in[len-i-1];
		if(current >= 'A' && current <= 'F'){
			t = current - 'A' + 10;
		} else if(current >= 'a' && current <= 'f'){
			t = current - 'A' - 22;
		}else{
			t = current - '0';
		}		
		out[index] |= t<< (4*(i%(BIT/4)));
	}
	out[0] = index;
}

void initArray(WORD *in, size_t size){
	for(WORD i = 0; i<size; i++){ // set array to zero
		in[i] = 0;
	}
}

void initArray8(uint8_t *in, size_t size){
	for(WORD i = 0; i<size; i++){ // set array to zero
		in[i] = 0;
	}
}

void convertArray16toArray8(uint8_t *out, uint16_t *in){
	WORD i;
	for(i=1; i<=in[0]; i++){
		out[i*2-1] = in[i];
		out[i*2] = in[i] >> 8;
	}
	WORD w = 2*in[0];
	while(out[w] == 0  && w>0){
		w--;
	}
	out[0] = w;
}

//added for debugging! need to clean up this code
void convertArray16toArray8withoutLen(uint8_t *out, uint16_t *in){
	WORD i;
	for(i=0; i<in[0]; i++){
		out[i*2] = in[i+1];
		out[i*2+1] = in[i+1] >> 8;
	}

//	WORD w = 2*in[0];
//	while (out[w] == 0  && w>0){
//		w--;
//	}
//	out[0] = w;
}


void convertArray8toArray16withoutLen(uint16_t *out, uint8_t *in, size_t len){
	WORD i;
	for(i=0; i<len; i++){
		out[i/2]=(((uint16_t)((in)[i])) | ((uint16_t)((in)[i+1]) << 8));
		i++;
	}

	//out[0] = in[0]/2;
}

/* transforms uint8_t array without length into an array of word (length = SIZE) with length field*/
void rawbyte2word(WORD *out, uint8_t *in, size_t len){
	assert((SIZE-1)*BYTE>= len); // to avoid that inside the uint8_t array there is too much to store in out
	uint16_t i = 0;
	uint16_t index = 0;
	WORD t = 0;
	initArray(out, SIZE);
	for(i=0; i<len; i++){
		index = i/(BIT/8)+1;
		t = in[i];
		out[index] |= t<< (8*(i%(BIT/8)));
	}
	while(out[index]==0 && index>0){
		index--;
	}
	out[0] = index;
}

/* transforms an array of word of length SIZE with a length field into an array of bytes without length. Out is maximum "max_len" long.
	The assert below treats a corner case where the byte array ends in the middle of the word array.*/
void word2rawbyte(uint8_t *out, WORD *in, size_t max_len){
	assert(in[0]*BYTE <= max_len); // to avoid overflow
	uint16_t i = 0;
	uint16_t index = 1;
	uint8_t t = 0;
	initArray8(out, max_len);
	while(i<max_len){
		t = in[index]>> (8*(i%(BIT/8)));
		out[i] = t;
		i++;
		index = i/(BIT/8)+1;	
	}
	if(i == max_len && index<=in[0] ){
		printf("Possibility that some partes of the last cell have not been correctly copied in function %s:\n", __func__);
		print_num(in);
		print_array8(out, max_len);
		P(max_len);
		assert(0);
	}
}

void convertArray8toArray16(uint16_t *out, uint8_t *in, size_t len){
	WORD i;
	for(i=0; i<in[0]; i++){
		out[i/2]=(((uint16_t)((in)[i])) | ((uint16_t)((in)[i+1]) << 8));
		i++;
	}

	out[0] = in[0]/2;
}



void convertWithSize(WORD *out, const char *in, size_t size){
	WORD len = strlen(in);

	for(WORD i = 0; i<SIZE; i++){ // set array to zero
		out[i] = 0;
	}

	WORD stopEarly = 0; // handle MSB zeros and remove them
	while( stopEarly<len && in[stopEarly] == '0'){
		stopEarly++;
	}
	WORD index = 0; 	// keeps track of the cells of the array
	WORD current = 0;	// will store the number as a char
	WORD t = 0;			// will store the number as an integer
	for(WORD i = 0; i<len-stopEarly ; i++){

		index = i/(size/4)+1;
		current = in[len-i-1];

		if(current >= 'A' && current <= 'F'){
			t = current - 'A' + 10;
		} else if(current >= 'a' && current <= 'f'){
			t = current - 'A' - 22;
		}else{
			t = current - '0';
		}

		out[index] |= t<< (4*(i%(size/4)));
	}
	out[0] = index;


}



/* 	Converts an number into an array of words.
   	INPUT:	number: can be any type, wil be casted to the largest uint64_t
	OUTPUT:	array : array of words, minimum length = 5 */
void number2array(WORD array[], uint64_t number){
	WORD i = 0;

	// 1. assign core value
	for (i = 1; i<=64/BIT; i++){
		array[i] = (WORD) number;
		number >>= BIT;
	}
	i--;

	// 2. assign length
	while(array[i] == 0 && i >0){  
        i--; 
    }	
	array[0] = i;
}

/*	Converts a string into an array of words.
	The last character '\0' is not converted.
	example: "hello" is converted in [0x03 0xlo 0xel 0x0h] (letters are replaced by ascii value)
   	INPUT:	text	: string of any length	
	OUTPUT:	array 	: array of words, length should be large enough to hold the string */
void text2array(WORD array[], char text[]){
    WORD i = 0;
	WORD index = 0;
	
	// 1. assign core value
	while(text[i] != '\0'){
        index = i/BYTE +1;	
        array[index] |= text[i] << ( (8*i) % (BIT) );
        i++;
    }
	index = (i-1)/BYTE +1; // decrement i once
	
	// 2. assign length
	while(array[index] == 0 && index >0){
        index--; 
    }	
    array[0] = index;
}

/*	Converts an array of words into a string.
	INPUT:	array 	: array of words, length should be large enough to hold the string 
	OUTPUT:	text	: string of any length	
*/
void array2text(char text[], WORD array[]){
    WORD i 		= 0;
	WORD index 	= 0;
	WORD len 	= getNumberBytes(array);
	
	while(i < len){
        index = i/BYTE +1;	
		text[i] = array[index] >> ( (8*i) % (BIT) );
        i++;
    }
	text[i] = '\0';
}




#if 0
/*s an array; format: [size, lsb-> msb] -- added but not used will check later if needs to be removed: sajetan*/
void print_num_size(WORD *in, size_t s){
    printf("[");
    for (WORD i = 0; i < s; i++) {
		switch(BIT){
			break;
			case 32:
    		printf("0x%08x,", in[i]);
			break;
			case 16:
    		printf("0x%04x,", in[i]);
			break;
			case 8:
    		printf("0x%02x,", in[i]);
			break;
			default:
    		printf("0x%08x,", in[i]);
			break;
		}
    }
    printf("]");
    printf("\n\r");
}


/*prints an array; format: [size, lsb-> msb] -- added but not used will check later if needs to be removed: sajetan*/
void print_num_size_type(WORD *in, size_t s, size_t type){
    printf("[");
    for (WORD i = 0; i < s; i++) {
		switch(type){
			break;
			case 32:
    		printf("0x%08x,", in[i]);
			break;
			case 16:
    		printf("0x%04x,", in[i]);
			break;
			case 8:
    		printf("0x%02x,", in[i]);
			break;
			default:
    		printf("0x%08x,", in[i]);
			break;
		}
    }
    printf("]");
    printf("\n\r");
}


#endif

/*prints an array; format: [size, lsb-> msb]*/
void print_num(WORD *in){
    printf("[");
    for (WORD i = 0; i < in[0]+1; i++) {
		switch(BIT){
			break;
			case 32:
    		printf("0x%08x,", in[i]);
			break;
			case 16:
    		printf("0x%04x,", in[i]);
			break;
			case 8:
    		printf("0x%02x,", in[i]);
			break;
			default:
    		printf("0x%08x,", in[i]);
			break;
		}
    }
    printf("]");
    printf("\n\r");
}
void print_hex(WORD *in)
{
    WORD i=0;

    printf("0x");
    for (i = in[0]; i >= 1; i--) {
    	switch(BIT){
			break;
			case 32:
    		printf("%08x", in[i]);
			break;
			case 16:
    		printf("%04x", in[i]);
			break;
			case 8:
    		printf("%02x", in[i]);
			break;
			default:
    		printf("%08x", in[i]);
			break;
    	}
    }
    printf("\n\r");
}

void print_hex_type(WORD *in, size_t type)
{
    WORD i = 0;
	if(type == 8){
		printf("info message: this is a uint8, not a word array=> zeros won't always be printed out properly");
	}
    printf("0x");
    for (i = in[0]; i >= 1; i--) {
    	switch(type){
			break;
			case 32:
    		printf("%08x", in[i]);
			break;
			case 16:
    		printf("%04x", in[i]);
			break;
			case 8:
    		printf("%02x", in[i]);
			break;
			default:
    		printf("%08x", in[i]);
			break;
    	}
    }
    printf("\n\r");
}

void print_hex_8(uint8_t *in)
{
    uint16_t i = 0;
    printf("0x");
    for (i = in[0]; i >= 1; i--) {
    	printf("%02x", in[i]);
    }
    printf("\n\r");
}

void print_hex_size_type(WORD *in,size_t size, size_t type)
{
    WORD i = 0;

    printf("0x");
    for (i = size; i >= 1; i--) {
    	switch(type){
			break;
			case 32:
    		printf("%08x", in[i]);
			break;
			case 16:
    		printf("%04x", in[i]);
			break;
			case 8:
    		printf("%02x", in[i]);
			break;
			default:
    		printf("%08x", in[i]);
			break;
    	}
    }
    printf("\n\r");
}


/*prints an array; format: [in[0], in[1], ..., in[size-1]]*/
void print_array(WORD *in, uint64_t size){
    printf("[");
    for (WORD i = 0; i < size; i++) {
		switch(BIT){
			break;
			case 32:
    		printf("0x%08x,", in[i]);
			break;
			case 16:
    		printf("0x%04x,", in[i]);
			break;
			case 8:
    		printf("0x%02x,", in[i]);
			break;
			default:
    		printf("0x%08x,", in[i]);
			break;
		}
    }
    printf("]");
    printf("\n\r");
}

/*prints an array; format: [in[0], in[1], ..., in[size-1]]*/
void print_array8(uint8_t *in, uint64_t size){
    printf("[");
    for (WORD i = 0; i < size; i++) {
    		printf("0x%02x,", in[i]);

    }
    printf("]");
    printf("\n\r");
}





/*below three are some fancy functions : sajetan */
/*
 * convert byte hex array to char array of type of 8/16/32bits
 * */
void byte2char(char *out, WORD *in, size_t type)
{
	size_t len=in[0];
	uint32_t i =0;
	if (type==8){
		while(i<len){sprintf(out+strlen(out),"%02x",in[len-i]);i++;}
	}
	else if (type==16){
		while(i<len){sprintf(out+strlen(out),"%04x",in[len-i]);i++;}
	}
	else if (type==32){
		while(i<len){sprintf(out+strlen(out),"%08x",in[len-i]);i++;}
	}
}

/*
 * convert byte hex array to char array of type of 8/16/32bits
 * */
void byte2charWithSize(char *out, uint8_t *in, size_t len,size_t type)
{
	uint32_t i =0;
	len=len-1;

	if (type==8){
		while(i<len+1){sprintf(out+strlen(out),"%02x",(uint8_t)in[len-i]);i++;}
	}
	else if (type==16){
		while(i<len+1){sprintf(out+strlen(out),"%04x",(uint16_t)in[len-i]);i++;}
	}
	else if (type==32+1){
		while(i<len){sprintf(out+strlen(out),"%08x",(uint32_t)in[len-i]);i++;}

	}
}


/*
 * convert char hex to 1 byte array
 * */
void char2byte(uint8_t *out, const char *in)
{
  while (*in) { sscanf(in, "%2hhx", out++); in += 2; }

}


/* copies array w to array copy
INPUT: 2 WORD[] arrays of length SIZE*/
void copyWord(WORD copy[], WORD w[]){
    for(WORD i = 0; i<SIZE; i++){
        copy[i] = w[i];
    }
}

/* copies array w to array copy
INPUT: 2 WORD[] arrays of length SIZE*/
void copyArrayWithSize(WORD copy[], WORD w[]){
    for(WORD i = 0; i<w[0]+1; i++){
        copy[i] = w[i];
    }
}

/* copies array w to array copy
INPUT: 2 WORD[] arrays of length len */
void copyArrayWithoutSize(WORD copy[], WORD w[], uint16_t len){
    for(uint16_t i = 0; i<len; i++){
        copy[i] = w[i];
    }
}

void copyBytes(uint8_t copy[], uint8_t w[], uint16_t len){
    for(uint16_t i = 0; i<len; i++){
        copy[i] = w[i];
    }
}


#if 0
//just added these functions will remove later while cleaning up code :sajetan
/* copies array w to array copy
INPUT: 2 WORD[] arrays of length SIZE
but doesnt include the length bit in the first array index
*/
void copyArrayWithoutLength(WORD copy[], WORD w[]){
    for(WORD i = 0; i<w[0]+1; i++){
        copy[i] = w[i+1];
    }
}

void copyArray(WORD copy[], WORD w[],size_t len){
    for(WORD i = 0; i<len; i++){
        copy[i] = w[i];
        printf("%02x ",copy[i]);
    }
}

#endif

/*function for comparing two WORDS[].
BEWARE: the lengreaterThanh field must be equal as well! 
e.g. {0} and {1, 0} are different 
INPUT: 2 WORD[] numbers
OUTPUT: 1 if equal, 0 if different*/
WORD equalWord(WORD a[], WORD b[]){
    WORD S1 = a[0];
    WORD S2 = b[0];
	
	// zero case
	if ((S1 == 1 || S1 == 0) && (S2 == 1 || S2 == 0) && (a[1] == 0) && (b[1] == 0)){
		return 1;
	}
    if(S1 != S2){
        return 0;

    }
    else
    {
        for(WORD i = 1; i<=S1; i++){
            if(a[i] != b[i]){
                return 0;
            }
        }
        return 1;
    }

}

/* prints a word[] in the following format:
"lengreaterThanh x MSB ... LSB" 
e.g. for 16 bit: "2 x 178B 002F"*/
void printWord(WORD a[]){
    printf("%d x ", a[0]);
    for(WORD i = a[0]; i>0; i--){
        printf("%x ", a[i]);
    }
    printf("\n");
}

WORD greaterThan(WORD a[], WORD b[]){
	WORD lenA = a[0];
	WORD lenB = b[0];
	if(lenA > lenB){
		return (WORD) 1;
	} else if(lenA < lenB){
		return (WORD) 0;
	} else if (lenA == 0){
		return (WORD) 0;
	} else{
		for(WORD i = lenA; i>0; i--){
			if(a[i]>b[i]){
				return (WORD) 1;
			} else if (a[i]<b[i]){
				return (WORD) 0;
			}
		}
		return (WORD) 0;
	}
}
/*
void updateLen(WORD a[]){
	uint16_t i = 0;
	assert(a[0]<=SIZE);
	for(i = a[0]; a[i]==0 && i>0; i--){
		a[0]--;
	}
}
*/



void convertTest(){
	WORD w[SIZE] = {0};
	printf("\n---- %s -----\n", __func__);
	// 6AB7F
	convert(w, "00000000000006ABF7000000000000000");
	printf("Original string: 00000000000006ABF7000000000000000 \n");
	printf("Resulting array: ");
	print_num(w);

	// 0
	convert(w, "0");
	printf("Original string: 0 \n");
	printf("Resulting array: ");
	print_num(w);

	// F0F0F0
	convert(w, "F0F0");
	printf("Original string: F0F0 \n");
	printf("Resulting array: ");
	print_num(w);

	// F0F0F0
	convert(w, "00F0F000");
	printf("Original string: 00F0F000 \n");
	printf("Resulting array: ");
	print_num(w);
	
	// F0F0F0
	convert(w, "00FFFF00");
	printf("Original string: 00FFFF00 \n");
	printf("Resulting array: ");
	print_num(w);

	convert(w, "abcdef");
	printf("Original string: abcdef \n");
	printf("Resulting array: ");
	print_num(w);

	convert(w, "ABCDEF");
	printf("Original string: ABCDEF \n");
	printf("Resulting array: ");
	print_num(w);

	convert(w, "cdbf000c000cf");
	printf("Original string: cdbf000c000cf \n");
	printf("Resulting array: ");
	print_num(w);


	

	printf("---------end %s-------------\n\n", __func__);
}

/* returns 1 if a>=b, 0 otherwise*/
WORD geq(WORD a[], WORD b[]){
	WORD lenA = a[0];
	WORD lenB = b[0];
	WORD i = lenA;

	if(lenA > lenB){
		return (WORD) 1;
	} else if(lenA < lenB){
		return (WORD) 0;
	} else if (lenA == 0){
		return (WORD) 1;
	} else{
		while(i>0){
			if(a[i]>b[i]){
				return (WORD) 1;
			} else if (a[i]<b[i]){
				return (WORD) 0;
			} else{
				i--;
			}
		}
		return (WORD) 1;
	}
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
