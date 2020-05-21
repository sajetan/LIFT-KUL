/*
 * chacha20_poly1305_interface.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 * This file is to integrate Chacha20 and Poly1305 for AEAD construction
 */

#include "chacha20.h"
#include "poly1305.h"
#include "chacha20_poly1305_interface.h"
#include "../common/main.h"
#include "aead_test.h"



//initializing encryption, this prototype can be simplified in next versions
void encrypt_init(u8 *ciphertext, u8 *key, u8 key_length, u8 *nonce, u8 *plaintext, u32 len, u64 counter)
{
	chacha20_ctx ctx,ctxd;
	chacha_init_ctx(&ctx);
	chacha_init_ctx(&ctxd);

	chacha20_init_setup(&ctx, key, key_length, nonce);
	chacha20_counter_set(&ctx, counter);
	chacha20_encrypt(&ctx, plaintext, ciphertext, len);
}


void poly1305_key_gen(uint8_t *poly_key, uint8_t *key,uint8_t *nonce ){
	chacha20_ctx ctx;
	chacha_init_ctx(&ctx); //init ctx

	uint8_t out[64]={0};
	chacha20_init_setup(&ctx, key, strlen((const char*)key), nonce); //block initial setup
	chacha20_counter_set(&ctx, 0x00000001);
	chacha20_block(&ctx, (uint32_t*) out); //one round of block operation

	memset(poly_key, 0x00,32);
	memcpy(poly_key, out, 32);

	//if(DEBUG) printf("[POLY KEY] - len %d - ",strlen(poly_key));print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
}


//refer to rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
void poly1305_msg_construction(u8 *poly_msg,poly1305_msg_struct *in){
	char poly_msg_array[MAX_MSG_SIZE*4];
	uint8_t msg_len[UINT64_SIZE],aad_len[UINT64_SIZE];

	memset(poly_msg_array,0x00,MAX_MSG_SIZE*4);

	memset(msg_len,0x00,UINT64_SIZE);
	memset(aad_len,0x00,UINT64_SIZE);

	*(uint64_t*)&msg_len=in->msg_len;  //convert uint64 to little endian of 8 bytes
	*(uint64_t*)&aad_len=in->aad_len;  //convert uint64 to little endian of 8 bytes

	if(DEBUG)printf("aad len %02x%02x%02x%02x%02x%02x%02x%02x \n",aad_len[0],aad_len[1],aad_len[2],aad_len[3],aad_len[4],aad_len[5],aad_len[6],aad_len[7]);
	if(DEBUG)printf("Message len %02x%02x%02x%02x%02x%02x%02x%02x \n",msg_len[0],msg_len[1],msg_len[2],msg_len[3],msg_len[4],msg_len[5],msg_len[6],msg_len[7]);

	//concatenating all aad+pad1+msg+pad2+aad_len+msg_len
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array), "%s", in->aad);
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array), "%s", in->pad1);
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array), "%s", in->msg);
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array), "%s", in->pad2);
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array),"%02x%02x%02x%02x%02x%02x%02x%02x",aad_len[0],aad_len[1],aad_len[2],aad_len[3],aad_len[4],aad_len[5],aad_len[6],aad_len[7]);
	sprintf(poly_msg_array+strlen((const char*)poly_msg_array),"%02x%02x%02x%02x%02x%02x%02x%02x",msg_len[0],msg_len[1],msg_len[2],msg_len[3],msg_len[4],msg_len[5],msg_len[6],msg_len[7]);

	in->total_len= strlen((const char*)poly_msg_array)/2;

	char2byte(poly_msg,poly_msg_array);

}

//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
//padding check- [AAD + pad1] [MSG + pad2] should be multiple of 16
void paddingCheck(poly1305_msg_struct  *in, uint16_t padding_length){
	WORD zeros_req1=0;
	WORD zeros_req2=0;
	zeros_req1 = padding_length-(strlen((const char*)in->aad)/2)%padding_length;
	if (zeros_req1 == padding_length)
		zeros_req1=0;

	zeros_req2 = padding_length-(strlen((const char*)in->msg)/2)%padding_length;
	if (zeros_req2 == padding_length)
		zeros_req2=0;


	in->msg_len=strlen((const char*)in->msg)/2;
	in->aad_len=strlen((const char*)in->aad)/2;
	while(zeros_req1 > 0){
		sprintf((char*)in->pad1+strlen((const char*)in->pad1),"%02x",0);
		zeros_req1--;
	}

	while(zeros_req2 > 0){
		sprintf((char*)in->pad2+strlen((const char*)in->pad2),"%02x",0);
		zeros_req2--;
	}


}



//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
void aead_chacha20_poly1305(uint8_t *output,uint8_t *ciphertext_out,uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len){
	poly1305_msg_struct poly_struct={{0}};
	uint64_t counter=0;
	uint8_t poly_msg[MAX_MSG_SIZE]={0};
	uint8_t poly_key[CHACHA_KEY_LENGTH]={0};

	char *aad = "50515253c0c1c2c3c4c5c6c7";

	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305

	encrypt_init(ciphertext_out,key, key_len, nonce, plaintext_in, len,counter); //encrypting plaintext

	if(DEBUG)print_num_type_length(ciphertext_out,len,8);


	memset(poly_struct.msg, 0x00, len);
	memset(poly_struct.aad,0x00,strlen((const char*)aad));

	memcpy(poly_struct.aad,aad,strlen((const char*)aad));

	byte2charWithSize((char*)poly_struct.msg, ciphertext_out, len,8); //convert ciphertext byte array to string array for padding check

	//below is from reference
	//	char test[]="d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116";
	//	memcpy(poly_struct.msg, test, strlen(test));

	if(DEBUG)printf("ciphertext = %s \n",poly_struct.msg);

	paddingCheck(&poly_struct, POLY_PADDING_LENGTH); //padding check
	poly1305_msg_construction(poly_msg, &poly_struct); //constructing message for calculating mac tag

	if(DEBUG){
		//printf("finished const len=%d \n",poly_struct.total_len);print_num_type(poly_msg, 8);print_num_type_length(poly_msg,poly_struct.total_len,8);
		//	printf("\nkey- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
		//	printf("\nnonce- ");print_num_type_length(nonce,8,8);
		//	printf("poly_key- ");print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
	}
	poly1305_auth(output, poly_msg, poly_struct.total_len, poly_key); //calculating mac tag
	if(DEBUG)printf("mac- ");
	if(DEBUG)print_num_type_length(output,MAC_TAG_LENGTH,8);
}


//rfc8439 section 2.8 https://tools.ietf.org/html/rfc8439#section-2.8
// Ferdinand: I changed this function such that it returns whether the mac is correct or not
uint16_t verify_mac_aead_chacha20_poly1305(uint8_t *rcv_mac_tag, uint8_t *key, uint32_t key_len, uint8_t *nonce, uint8_t *plaintext_in, uint32_t len){
	poly1305_msg_struct poly_struct={{0}};
	uint8_t poly_msg[MAX_MSG_SIZE]={0};
	uint8_t mac_tag[MAC_TAG_LENGTH]={0};
	uint8_t poly_key[CHACHA_KEY_LENGTH]={0};
	uint16_t valid = 0;
	char *aad = "50515253c0c1c2c3c4c5c6c7";

	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305

	memset(poly_struct.msg, 0x00, len);
	memset(poly_struct.aad,0x00,strlen((const char*)aad));
	memcpy(poly_struct.aad,aad,strlen((const char*)aad));
	byte2charWithSize((char*)poly_struct.msg, plaintext_in, len,8); //convert ciphertext byte array to string array for padding check

	//below is from reference
	//	char test[]="d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116";
	//	memcpy(poly_struct.msg, test, strlen(test));


	paddingCheck(&poly_struct, POLY_PADDING_LENGTH); //padding check
	poly1305_msg_construction(poly_msg, &poly_struct); //constructing message for calculating mac tag

	if(DEBUG){
		//	printf("finished const len=%d \n",poly_struct.total_len);print_num_type(poly_msg, 8);print_num_type_length(poly_msg,poly_struct.total_len,8);
		//	printf("\nkey- ");print_num_type_length(key,CHACHA_KEY_LENGTH,8);
		//	printf("\nnonce- ");print_num_type_length(nonce,8,8);
		//	printf("poly_key- ");print_num_type_length(poly_key,CHACHA_KEY_LENGTH,8);
	}
	poly1305_auth(mac_tag, poly_msg, poly_struct.total_len, poly_key); //calculating mac tag
	if(DEBUG){
		//	printf("mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
		//	printf("rcvmac- ");print_num_type_length(rcv_mac_tag,MAC_TAG_LENGTH,8);
	}
	valid = poly1305_verify(mac_tag, rcv_mac_tag);
	if(DEBUG)printf(" ---------------------------  MAC VERIFICATION [ %d ] -------------------------------------\n", valid);
	return valid;
}

/**
 * Test functions
 *
 */

void aead_test(){
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};

	uint8_t key[CHACHA_KEY_LENGTH]={0};
	uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
	uint8_t plaintext_in[MAX_MSG_SIZE]={0};
	uint8_t ciphertext[MAX_MSG_SIZE]={0};

	char2byte(key,gkey);
	char2byte(nonce, gnonce);
	char2byte(plaintext_in, gplain);
	uint8_t plaintext_length=strlen((const char*)gplain)/2;

	aead_chacha20_poly1305(mac_tag,ciphertext, key, 32, nonce, plaintext_in, plaintext_length);
	printf("mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("ciphertext- ");print_num_type_length(ciphertext,plaintext_length,8);


}


void padding_test(){
	char aad[]="50515253c0c1c2c3c4c5c6c7";
	char ciphertext[]="1234567089";
	char k[]="808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f";
	char n[]="4041424344454647";
	uint8_t key[32]={0};
	uint8_t nonce[8]={0};
	uint8_t poly_key[32]={0};
	uint8_t input[MAX_MSG_SIZE] = {0};
	uint8_t mac[16]={0};
	uint8_t output[MAX_MSG_SIZE] = {0};
	poly1305_msg_struct inp={{0}};

	char2byte(input,aad);

	printf("-------------------------");print_num_type(output,8);

	memset(inp.aad,0x00,strlen((const char*)inp.aad));
	memset(inp.msg,0x00,strlen((const char*)inp.aad));
	memset(inp.pad1,0x00,strlen((const char*)inp.aad));
	memset(inp.pad2,0x00,strlen((const char*)inp.aad));

	memcpy(inp.aad, aad, strlen((const char*)aad));
	memcpy(inp.msg,ciphertext,strlen((const char*)ciphertext));

	paddingCheck(&inp,16);
	uint8_t poly_msg[]={0};
	poly1305_msg_construction(poly_msg, &inp);

	char2byte(key,k);
	char2byte(nonce, n);

	poly1305_key_gen(poly_key, key,nonce); //generate one time key for poly1305
	print_num_type(poly_key,8);
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

	char2byte(key,k);
	char2byte(nonce, n);
	char2byte(plaintext, msg);

	decrypt_init(ciphertext_out,key, strlen((const char*)k)/2, nonce, plaintext,strlen((const char*)msg)/2, ctr);
	printf("new encrypt - "); print_num_type(ciphertext_out,8);
	printf("orig - ");print_num_type(plaintext,8);
}
