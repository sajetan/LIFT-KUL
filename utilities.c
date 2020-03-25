/*
 * utilities.c
 */

#include<stdio.h>
#include<stdint.h>
#include<math.h>
#include<string.h>


//WORD generate_nonce(WORD *nonce, size_t size);


void hex32_decoder(const char *in, size_t len,WORD *out){
	WORD padding_required=0;
	WORD padzeros=0;
	WORD i, t, bit0,bit1,bit2,bit3,bit4,bit5,bit6,bit7, index;

	if (len%8!=0) {padding_required=1;padzeros=8-len%8;}
	len=ceil(len/8.0);
	for (t = len,i = 0; i < len*8-padzeros; i+=8,--t){
		if (padding_required){
			WORD t_arr[8]={0};
			for (WORD offset = padzeros,index=0; offset < 8;offset++,index++ ){
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

void hex16_decoder(const char *in, size_t len,WORD *out){
        WORD padding_required=0;
        WORD padzeros=0;
        WORD i, t, bit0,bit1,bit2,bit3, index;

        if (len%4!=0) {padding_required=1;padzeros=4-len%4;}
        len=ceil(len/4.0);
        for (t = len,i = 0; i < len*4-padzeros; i+=4,--t){
                if (padding_required){
                        WORD t_arr[4]={0};
                        for (WORD offset = padzeros,index=0; offset < 4;offset++,index++ ){
                                t_arr[offset]=in[index] > '9' ? toupper(in[index]) - 'A' + 10 : in[index] - '0';
                        }
                        out[t] =  (t_arr[0]<<12) | (t_arr[1]<<8) | (t_arr[2]<<4) | t_arr[3];
                        padding_required=0;
                        i-=padzeros;
                }
                else{
                        bit3 = in[i] > '9' ? toupper(in[i]) - 'A' + 10 : in[i] - '0';
                        bit2 = in[i+1] > '9' ? toupper(in[i+1]) - 'A' + 10 : in[i+1] - '0';
                        bit1 = in[i+2] > '9' ? toupper(in[i+2]) - 'A' + 10 : in[i+2] - '0';
                        bit0 = in[i+3] > '9' ? toupper(in[i+3]) - 'A' + 10 : in[i+3] - '0';

                        out[t] =  (bit3<<12) | (bit2<<8) | (bit1<<4) | bit0;
                }
        }
        out[0]=len;
}

void hex8_decoder(const char *in, size_t len,WORD *out){
        WORD padding_required=0;
        WORD padzeros=0;
        WORD i, t, bit0,bit1,index;

        if (len%2!=0) {padding_required=1;padzeros=2-len%2;}
        len=ceil(len/2.0);
        for (t = len,i = 0; i < len*2-padzeros; i+=2,--t){
                if (padding_required){
                        WORD t_arr[2]={0};
                        for (WORD offset = padzeros,index=0; offset < 2;offset++,index++ ){
                                t_arr[offset]=in[index] > '9' ? toupper(in[index]) - 'A' + 10 : in[index] - '0';
                        }
                        out[t] = (t_arr[0]<<4) | t_arr[1];
                        padding_required=0;
                        i-=padzeros;
                }
                else{
                        bit1 = in[i] > '9' ? toupper(in[i]) - 'A' + 10 : in[i] - '0';
                        bit0 = in[i+1] > '9' ? toupper(in[i+1]) - 'A' + 10 : in[i+1] - '0';

                        out[t] =  (bit1<<4) | bit0;
                }
        }
        out[0]=len;
}


void print_num(WORD *in, WORD size){
    WORD i;
    printf("size= %d\n",size);
    printf("[");
    for (i = 0; i < size+1; i++) {
    	printf("0x%08x,", in[i]);
    }
    printf("]");
    printf("\n\r");
}



void test_convert_hex_array(){
	char p[]= "10202992191299292273736782983983873766292309238287371723929392388299190902901277462346";
	WORD c[1024]={0};
	hex32_decoder(p,strlen(p), c);
	print_num(c, c[0]);
	memset(c,0x00,1024);
	hex16_decoder(p,strlen(p), c);
	print_num(c, c[0]);
	memset(c,0x00,1024);
	hex8_decoder(p,strlen(p), c);
	print_num(c, c[0]);

}


