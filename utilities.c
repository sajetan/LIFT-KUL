/*
 * utilities.c
 */

#include"utilities.h"

/*
Converts a string of a hexadecimal numbers into an array of integers.

out[0] contains the length
out[1] is the lsb
out[out[0]] is the msb
example: suppose that WORD is uint8_t:
in: "8F59B"
out: [3, 9B, F5, 8]
!!! out[] must have size SIZE and the string must be shorter that BIT/4*SIZE-1*/
void convert(WORD *out, const char *in){
	for(WORD i = 0; i<sizeof(out)/sizeof(WORD); i++){ // set array to zero
		out[i] = 0;
	}

	WORD len = strlen(in);
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
/*prints an array; format: [size, lsb-> msb]*/
void print_num(WORD *in){
    //printf("[size, lsb-> msb]:  ");
    printf("[");
    for (WORD i = 0; i < in[0]+1; i++) {
		switch(BIT){
			case 64:
    		printf("0x%16x,", in[i]);
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


/* copies array w to array copy
INPUT: 2 WORD[] arrays of length SIZE*/
void copyWord(WORD copy[], WORD w[]){
    for(WORD i = 0; i<=SIZE; i++){
        copy[i] = w[i];
    }
}

/*function for comparing two WORDS[].
BEWARE: the length field must be equal as well! 
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
"length x MSB ... LSB" 
e.g. for 16 bit: "2 x 178B 002F"*/
void printWord(WORD a[]){
    printf("%d x ", a[0]);
    for(WORD i = a[0]; i>0; i--){
        printf("%x ", a[i]);
    }
    printf("\n");
}

void hex_decoder(const char *in, size_t len,uint32_t *out){
	uint32_t padding_required=0;
	uint32_t padzeros=0;
	uint32_t i, t, bit0,bit1,bit2,bit3,bit4,bit5,bit6,bit7, index;

	if (len%8!=0) {padding_required=1;padzeros=8-len%8; len = len/8+1;}
	//len=len/8+1; // I removed ceil() because even with the math.h library, it gave me an error, I didn't
	for (t = len,i = 0; i < len*8-padzeros; i+=8,--t){
		if (padding_required){
			uint32_t t_arr[8]={0};
			for (uint32_t offset = padzeros,index=0; offset < 8;offset++,index++ ){
				t_arr[offset]=in[index] > '9' ? toupper(in[index]) - 'A' + 10 : in[index] - '0';
			}
			out[t] = (t_arr[0]<<28)| (t_arr[1]<<24) | (t_arr[2]<<20) | (t_arr[3]<<16) | (t_arr[4]<<12) | (t_arr[5]<<8) | (t_arr[6]<<4) | t_arr[7];
			padding_required=0;
			i-=padzeros;
		}
		else{
		bit7 = in[i] > '9' ? toupper(in[i]) - 'A' + 10 : in[i] - '0';
		bit6 = in[i+1] > '9' ? toupper(in[i+1]) - 'A' + 10 : in[i+1] - '0';
		bit5 = in[i+2] > '9' ? toupper(in[i+2]) - 'A' + 10 : in[i+2] - '0';
		bit4 = in[i+3] > '9' ? toupper(in[i+3]) - 'A' + 10 : in[i+3] - '0';
		bit3 = in[i+4] > '9' ? toupper(in[i+4]) - 'A' + 10 : in[i+4] - '0';
		bit2 = in[i+5] > '9' ? toupper(in[i+5]) - 'A' + 10 : in[i+5] - '0';
		bit1 = in[i+6] > '9' ? toupper(in[i+6]) - 'A' + 10 : in[i+6] - '0';
		bit0 = in[i+7] > '9' ? toupper(in[i+7]) - 'A' + 10 : in[i+7] - '0';

		out[t] = (bit7<<28)| (bit6<<24) | (bit5<<20) | (bit4<<16) | (bit3<<12) | (bit2<<8) | (bit1<<4) | bit0;
		}
	}
	out[0]=len;
}



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

	printf("---------end %s-------------\n\n", __func__);
}

void test_convert_hex_array(){
	char p[]= "10";
	uint32_t c[1024]={0};
	hex_decoder(p,strlen(p), c);
	print_num(c);
}