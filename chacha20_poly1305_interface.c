/*
 * chacha20_poly1305_interface.c
 * This file is to integrate Chacha20 and Poly1305 for AEAD construction
 * sajetan
 */

#include "chacha20.h"
//#include "poly1305-donna.h"
#include"chacha20_poly1305_interface.h"
#include "main.h"
#include "aead_test.h"


#define decrypt_init encrypt_init //encryption and decryption uses the same code


//initializing encryption, this prototype can be simplified in next versions
void encrypt_init(u8 *ciphertext, u8 *key, u8 key_length, u8 *nonce, u8 *plaintext, u8 len, u64 counter)
{
	chacha20_ctx ctx,ctxd;
	chacha_init_ctx(&ctx);
	chacha_init_ctx(&ctxd);

	size_t i = 0;
	//uint8_t key[32];
	//uint8_t nonce[8];
//	uint8_t key_length = sizeof(key); printf("kkey");P(key_length);//key_in[0];
//	size_t len = sizeof(plaintext)/sizeof(uint8_t); P(len);//plaintext_in[0];


	//	  char key_arr[32]={0};
	//    char nonce_arr[8]={0};
	//	  char plaintext_arr[MAX_MSG_SIZE] = {0};

	//ugly code to support the library reference --dont remove this
	//	  byte2char(key_arr, key_in,sizeof(WORD)*8);
	//	  byte2char(nonce_arr, nonce_in,sizeof(WORD)*8);
	//	  byte2char(plaintext_arr, plaintext_in,sizeof(WORD)*8);

	// printf("key- %s \n non- %s \npt- %s\n", key_arr, nonce_arr,plaintext_arr);


	//ugly code to support the library reference --dont remove this
	//	  char2byte(key,key_arr);
	//	  char2byte(nonce, nonce_arr);
	//	  char2byte(plaintext, plaintext_arr);

	chacha20_init_setup(&ctx, key, key_length, nonce);
	chacha20_counter_set(&ctx, counter);
	chacha20_encrypt(&ctx, plaintext, ciphertext, len);

//	P(strlen(ciphertext));printf("encrypt - "); print_num_type(ciphertext,8);//print_num_size(ciphertext,len);
//	printf("orig - ");print_num_type(plaintext,8);
//	uint8_t output[MAX_MSG_SIZE]={0};

//	chacha20_init_setup(&ctxd, key, key_length, nonce);
//	chacha20_counter_set(&ctxd, counter);
//	chacha20_decrypt(&ctxd, ciphertext, output, len);

//	printf("decrypt - ");print_num_type(output,8);
//	printf("orig - ");print_num_type(ciphertext,8);
}



void print_num_type(uint8_t *in,size_t type){
	printf("[");
    for (uint8_t i = 0; i < strlen(in); i++) printf("0x%02x,", in[i]);
    printf("]");
    printf("\n\r");
}

void print_num_type_length(uint8_t *in,size_t len,size_t type){
	printf("[");
    for (uint8_t i = 0; i < len; i++) printf("0x%02x,", in[i]);
    printf("]");
    printf("\n\r");
}


void poly1305_key_gen(uint8_t *poly_key, uint8_t *key,uint8_t *nonce ){
	chacha20_ctx ctx;
	chacha_init_ctx(&ctx); //init ctx

	uint8_t out[64]={0};
	chacha20_init_setup(&ctx, key, strlen(key), nonce); //block initial setup
	chacha20_counter_set(&ctx, 0x700000000);
	chacha20_block(&ctx, out); //one round of block operation

	memset(poly_key, 0x00,32);
	memcpy(poly_key, out, 32);

	printf("[POLY KEY] - len %d - ",strlen(poly_key));print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
}


//refer to rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
void poly1305_msg_construction(u8 *poly_msg,poly1305_msg_struct *in){
	char poly_msg_array[MAX_MSG_SIZE*4];
	uint8_t msg_len[UINT64_SIZE],aad_len[UINT64_SIZE];

	memset(poly_msg_array,0x00,MAX_MSG_SIZE*4);

	memset(msg_len,0x00,strlen(msg_len));
	memset(aad_len,0x00,strlen(aad_len));

	*(uint64_t*)&msg_len=in->msg_len;  //convert uint64 to little endian of 8 bytes
	*(uint64_t*)&aad_len=in->aad_len;  //convert uint64 to little endian of 8 bytes

//	printf("aad len %02x%02x%02x%02x%02x%02x%02x%02x \n",aad_len[0],aad_len[1],aad_len[2],aad_len[3],aad_len[4],aad_len[5],aad_len[6],aad_len[7]);
//	printf("message len %02x%02x%02x%02x%02x%02x%02x%02x \n",msg_len[0],msg_len[1],msg_len[2],msg_len[3],msg_len[4],msg_len[5],msg_len[6],msg_len[7]);



	//concatenating all aad+pad1+msg+pad2+aad_len+msg_len
	sprintf(poly_msg_array+strlen(poly_msg_array), "%s", in->aad);
	sprintf(poly_msg_array+strlen(poly_msg_array), "%s", in->pad1);
	sprintf(poly_msg_array+strlen(poly_msg_array), "%s", in->msg);
	sprintf(poly_msg_array+strlen(poly_msg_array), "%s", in->pad2);
	sprintf(poly_msg_array+strlen(poly_msg_array),"%02x%02x%02x%02x%02x%02x%02x%02x",aad_len[0],aad_len[1],aad_len[2],aad_len[3],aad_len[4],aad_len[5],aad_len[6],aad_len[7]);
	sprintf(poly_msg_array+strlen(poly_msg_array),"%02x%02x%02x%02x%02x%02x%02x%02x",msg_len[0],msg_len[1],msg_len[2],msg_len[3],msg_len[4],msg_len[5],msg_len[6],msg_len[7]);

	in->total_len= strlen(poly_msg_array)/2;

	char2byte(poly_msg,poly_msg_array);

}

//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
//padding check- [AAD + pad1] [MSG + pad2] should be multiple of 16
void paddingCheck(poly1305_msg_struct  *in, uint8_t padding_length){
	uint8_t zeros_req1=0;
	uint8_t zeros_req2=0;
	zeros_req1 = padding_length-(strlen(in->aad)/2)%padding_length;
	if (zeros_req1 == padding_length)
		zeros_req1=0;


	zeros_req2 = padding_length-(strlen(in->msg)/2)%padding_length;
	if (zeros_req2 == padding_length)
		zeros_req2=0;


	in->msg_len=strlen(in->msg)/2;
	in->aad_len=strlen(in->aad)/2;
//	printf("########aad pad1 len %d %s %d pad[ %s %d ] aad len = [ %d ]\n", zeros_req1, in->aad, strlen(in->aad), in->pad1, strlen(in->pad1), in->aad_len);
	while(zeros_req1 > 0){
		sprintf(in->pad1+strlen(in->pad1),"%s","00");
		zeros_req1--;
	}

//	printf("########3mesg pad2 len %d %s %d pad[ %s %d ] mesg len = [ %d ]\n", zeros_req2, in->msg, strlen(in->msg), in->pad2, strlen(in->pad2),in->msg_len);
//	printf("value of zeros in message = [%d] \n",zeros_req2);
	while(zeros_req2 > 0){
		sprintf(in->pad2+strlen(in->pad2),"%s","00");
		zeros_req2--;
	}
//	printf("########aad pad1 len %d %s %d pad[ %s %d ] aad len = [ %d ]\n", zeros_req1, in->aad, strlen(in->aad), in->pad1, strlen(in->pad1), in->aad_len);
//	printf("########3mesg pad2 len %d %s %d pad[ %s %d ] mesg len = [ %d ]\n", zeros_req2, in->msg, strlen(in->msg), in->pad2, strlen(in->pad2),in->msg_len);


}



//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
void aead_chacha20_poly1305(uint8_t *output,uint8_t *ciphertext_out,uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len, const char *aad){
	poly1305_msg_struct poly_struct={0};
	uint64_t counter=0;
	uint8_t poly_msg[MAX_MSG_SIZE]={0};
	uint8_t poly_key[CHACHA_KEY_LENGTH]={0};

	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305

	encrypt_init(ciphertext_out,key, key_len, nonce, plaintext_in, len,counter); //encrypting plaintext

	//print_num_type_length(ciphertext_out,len,8);


	memset(poly_struct.msg, 0x00, len);
	memset(poly_struct.aad,0x00,strlen(aad));

	memcpy(poly_struct.aad,aad,strlen(aad));

	byte2charWithSize(poly_struct.msg, ciphertext_out, len,8); //convert ciphertext byte array to string array for padding check

//below is from reference
//	char test[]="d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116";
//	memcpy(poly_struct.msg, test, strlen(test));

//	printf("##############cipher text = %s \n",poly_struct.msg);

	paddingCheck(&poly_struct, POLY_PADDING_LENGTH); //padding check
 	poly1305_msg_construction(poly_msg, &poly_struct); //constructing message for calculating mac tag
//	printf("##################finished const len=%d \n",poly_struct.total_len);print_num_type(poly_msg, 8);print_num_type_length(poly_msg,poly_struct.total_len,8);

	printf("\n@@@@@@ key- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
	printf("\n@@@@@@ nonce- ");print_num_type_length(nonce,8,8);
	printf("@@@@@@ poly_key- ");print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
	poly1305_auth(output, poly_msg, poly_struct.total_len, poly_key); //calculating mac tag
	printf("@@@@@@ mac- ");print_num_type_length(output,MAC_TAG_LENGTH,8);
}


//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
void verify_mac_aead_chacha20_poly1305(uint8_t *rcv_mac_tag, uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len, const char *aad){
	poly1305_msg_struct poly_struct={0};
	uint64_t counter=0;
	uint8_t poly_msg[MAX_MSG_SIZE]={0};
	uint8_t mac_tag[MAC_TAG_LENGTH]={0};
	uint8_t poly_key[CHACHA_KEY_LENGTH]={0};

	printf("\n@@@@@@ key1- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305

	memset(poly_struct.msg, 0x00, len);
	memset(poly_struct.aad,0x00,strlen(aad));
	memcpy(poly_struct.aad,aad,strlen(aad));
	printf("\n@@@@@@ key2- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
	byte2charWithSize(poly_struct.msg, plaintext_in, len,8); //convert ciphertext byte array to string array for padding check

//below is from reference
//	char test[]="d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116";
//	memcpy(poly_struct.msg, test, strlen(test));

//	printf("##############cipher text = %s \n",poly_struct.msg);

	paddingCheck(&poly_struct, POLY_PADDING_LENGTH); //padding check
 	poly1305_msg_construction(poly_msg, &poly_struct); //constructing message for calculating mac tag
//	printf("##################finished const len=%d \n",poly_struct.total_len);print_num_type(poly_msg, 8);print_num_type_length(poly_msg,poly_struct.total_len,8);
	printf("\n@@@@@@ key- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
	printf("\n@@@@@@ nonce- ");print_num_type_length(nonce,8,8);
	printf("@@@@@@ poly_key- ");print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
	poly1305_auth(mac_tag, poly_msg, poly_struct.total_len, poly_key); //calculating mac tag
	printf("@@@@@@ mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("@@@@@ rcvmac- ");print_num_type_length(rcv_mac_tag,MAC_TAG_LENGTH,8);
	int val = poly1305_verify(mac_tag, rcv_mac_tag);
	printf("@@@@@ ---------------------------  MAC VERIFICATION [ %d ] \n", val);

}

/**
 * Test functions
 *
 */


void aead_test(){
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};

    uint8_t key[CHACHA_KEY_LENGTH]={0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
    uint8_t poly_key[CHACHA_KEY_LENGTH]={0};
    uint8_t plaintext_in[MAX_MSG_SIZE]={0};
    uint8_t ciphertext[MAX_MSG_SIZE]={0};

    char2byte(key,gkey);
	char2byte(nonce, gnonce);
	char2byte(plaintext_in, gplain);
    uint8_t plaintext_length=strlen(gplain)/2;

	aead_chacha20_poly1305(mac_tag,ciphertext, key, 32, nonce, plaintext_in, plaintext_length, gaad);
	printf("mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("ciphertext- ");print_num_type_length(ciphertext,plaintext_length,8);


}


void padding_test(){
	char aad[]="50515253c0c1c2c3c4c5c6c7";
	char ciphertext[]="1234567089";
	uint8_t input[MAX_MSG_SIZE] = {0};
	char2byte(input,aad);
	uint8_t output[MAX_MSG_SIZE] = {0};

	printf("-------------------------");print_num_type(output,8);
	poly1305_msg_struct inp={0};
	memset(inp.aad,0x00,strlen(inp.aad));
	memset(inp.msg,0x00,strlen(inp.aad));
	memset(inp.pad1,0x00,strlen(inp.aad));
	memset(inp.pad2,0x00,strlen(inp.aad));

	memcpy(inp.aad, aad, strlen(aad));
	memcpy(inp.msg,ciphertext,strlen(ciphertext));

	paddingCheck(&inp,16);
	printf("pad1 %d [%s]\n", strlen(inp.pad1), inp.pad1);
	printf("pad2 %d [%s]\n", strlen(inp.pad2), inp.pad2);

	uint8_t poly_msg[]={0};
	poly1305_msg_construction(poly_msg, &inp);
	printf("setup key = %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x \n",poly_msg[0],poly_msg[1],poly_msg[2],poly_msg[3],poly_msg[4],poly_msg[5],poly_msg[6],poly_msg[7], poly_msg[8],poly_msg[9],poly_msg[10],poly_msg[11],poly_msg[12],poly_msg[13],poly_msg[14],poly_msg[15]);
	printf("setup key = %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x \n",poly_msg[16],poly_msg[17],poly_msg[18],poly_msg[19],poly_msg[20],poly_msg[21],poly_msg[22],poly_msg[23], poly_msg[24],poly_msg[25],poly_msg[26],poly_msg[27],poly_msg[28],poly_msg[29],poly_msg[30],poly_msg[31]);


	char k[]="808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f";
	char n[]="4041424344454647";
    uint8_t key[32]={0};
    uint8_t nonce[8]={0};
    uint8_t poly_key[32]={0};
	char2byte(key,k);
	char2byte(nonce, n);

	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305
	print_num_type(poly_key,8);

	uint8_t mac[16]={0};

	poly1305_auth( mac, poly_msg, inp.total_len, poly_key);
	printf("mac- ");print_num_type(mac,8);
}




void test_poly_key(){
	char k[]="808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f";
	char n[]="0001020304050607";
    uint8_t key[32]={0};
    uint8_t nonce[8]={0};
    uint8_t output[32]={0};
	char2byte(key,k);
	char2byte(nonce, n);

	poly1305_key_gen(output, key,nonce); //generate one time key for poly1305
	print_num_type(output,8);
}


void test_ec(){
	char k[]="808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f";
	char n[]="4041424344454647";
	char msg[]="4c616469657320616e642047656e746c656d656e206f662074686520636c617373206f66202739393a204966204920636f756c64206f6666657220796f75206f6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73637265656e20776f756c642062652069742e";
	u64 ctr=0;

	uint8_t key[32]={0};
	uint8_t nonce[8]={0};
	uint8_t plaintext[MAX_MSG_SIZE] = {0};
	uint8_t ciphertext_out[MAX_MSG_SIZE] = {0};

//    convert(key, k);
//    convert(nonce, n);
//    convert(plaintext, msg);

//    print_num(nonce);
//    print_num(plaintext);
//    print_num(key);

	char2byte(key,k);
	char2byte(nonce, n);
	char2byte(plaintext, msg);


	decrypt_init(ciphertext_out,key, strlen(k)/2, nonce, plaintext,strlen(msg)/2, ctr);
	P(sizeof(ciphertext_out)/sizeof(uint8_t));printf("new encrypt - "); print_num_type(ciphertext_out,8);//print_num_size(ciphertext,len);
	printf("orig - ");print_num_type(plaintext,8);

}
