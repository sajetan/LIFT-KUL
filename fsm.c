/*
 * fsm.c
 * 
 *  Created on: April 18, 2020
 *      Author: Ferdinand Hannequart
 */

#include"fsm.h"

void initMemory(Memory* mem){

    mem->receiverID = 117;
    initPool(&mem->pool);
    accumulate(&mem->pool, 100);
    initArray(mem->SK,SIZE);
    initArray(mem->PKCX,SIZE);
    initArray(mem->PKCY,SIZE);
    initArray(mem->PKDX,SIZE);
    initArray(mem->PKDY,SIZE);
    initArray(mem->GX,SIZE);
    initArray(mem->GY,SIZE);
    initArray(mem->N,SIZE);
    initArray(mem->SESSION_KEY,32);

    convert(mem->GX, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
    convert(mem->GY, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
    convert(mem->N,  "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");

}



State STS_send_0_fct(uint8_t* buf, Memory* mem){
    printf("\n/////\n State STS_send_0 \n");

    // compute new message : 
    uint8_t data[64] = {0};    
    make_STS_0_data(data, mem);    //STS_0_data = pointc.x||pointc.y 

    // make the buffer
	WORD_LEN len = 64;
    uint16_t buf_len = 0;
    getTLV(buf, &buf_len, TAG_STS_0, len, mem->receiverID, data);

    // send message
    if(send_message(buf, buf_len)==0) {
		close_sockets();
		printf("Send failed at message tag %d\n", TAG_STS_0);
		return 0;
	}

    // print sent message
	printf("Sent message STS 0:    \n");
	for(int i = 0; i<buf_len; i++){
		printf("%x ", buf[i]);
	}
    return STS_send_2;
}

State STS_send_2_fct(uint8_t* buf, Memory* mem){
    printf("\n//////\nState STS_send_2 \n");

    //prepare variables to store data
    uint16_t buf_len = 0;
    uint16_t rcv_buf_len;
    WORD_LEN rcv_data_len;
    WORD_ID rcv_id;
    WORD_TAG rcv_tag;
    uint8_t rcv_data[MAX_DATA_LENGTH];

    // initialize buffer (otherwise it contains precedent data)
    for(int i = 0; i<MAX_TRANSFER_LENGTH; i++){
        buf[i] = 0;
    }

    //receive message
    rcv_buf_len = receive_message(buf);
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, buf_len);

    // print received message
    printf("Received STS 1: \n");
    printf("\t tag = %d\n", rcv_tag);
    printf("\t len = %d\n", rcv_data_len);
    printf("\t id  = %d\n", rcv_id);
    printf("\t data = ");
	for(int i = 0; i<rcv_data_len; i++){
		printf("%x ", rcv_data[i]);
	}
    printf("\n");

    if(rcv_tag != TAG_STS_1){
        printf("Invalid tag due to timeout or error, return in first state\n");
        return STS_send_0;
    }
    else{
        printf("Valid tag, verify STS_1\n");
        
        //verify STS_1
        uint8_t valid_STS1 = 1;

        valid_STS1 = verify_STS_1(rcv_data, mem);

        if(valid_STS1 == 0){
            printf("STS_1 invalid, return in first state\n");
            return STS_send_0;
        }

        else{

            printf("Valid STS_1, send next message\n");
           
            // compute new message
            
            uint8_t data[64] = {0}; 

            make_STS_2_data(data,rcv_data, mem);

            WORD_LEN len = 92;                  // making the buf will change this in next version
            getTLV(buf, &buf_len, TAG_STS_2, len, mem->receiverID, data);

            // send message
            if(send_message(buf, buf_len)==0) {
                close_sockets();
                printf("Send failed at message STS2 \n");
                return 0;
            }
            
            // print sent message
            printf("Sent message STS 2:    ");
            for(int i = 0; i<buf_len; i++){
                printf("%x ", buf[i]);
            }

            return STS_receive_OK;

        }
    }
}

State STS_send_1_fct(uint8_t* buf, Memory* mem){
    printf("\n//////\nState STS_send_1 \n");

    //prepare variables to store data
    uint16_t buf_len = 0;
    uint16_t rcv_buf_len;
    WORD_LEN rcv_data_len;
    WORD_ID rcv_id;
    WORD_TAG rcv_tag;
    uint8_t rcv_data[MAX_DATA_LENGTH];

    // initialize buffer (otherwise it contains precedent data)
    for(int i = 0; i<MAX_TRANSFER_LENGTH; i++){
        buf[i] = 0;
    }

    //receive message
    rcv_buf_len = receive_message(buf);
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, rcv_buf_len);

    // print received message
    printf("Received STS 0: \n");
    printf("\t tag = %d\n", rcv_tag);
    printf("\t len = %d\n", rcv_data_len);
    printf("\t id  = %d\n", rcv_id);
    printf("\t data = ");
	for(int i = 0; i<rcv_data_len; i++){
		printf("%x ", rcv_data[i]);
	}
    printf("\n");

    if(rcv_tag != TAG_STS_0){
        printf("Invalid tag due to timeout or error, return in first state\n");
        return STS_drone_listening;
    }
    else{
        printf("Valid tag, send next message\n");
        // compute new message
        uint8_t data[MAX_DATA_LENGTH] = {0};
       
        make_STS_1_data(data, rcv_data, mem);

        WORD_LEN len = 158;                  // making the buf sajetan: needs to get this from the function
        getTLV(buf, &buf_len, TAG_STS_1, len, mem->receiverID, data);    
        

        // send message
        if(send_message(buf, buf_len)==0) {
            close_sockets();
            printf("Send failed at message STS1 \n");
            return 0;
        }
        
        // print sent message
        printf("Sent message STS 1:    ");
        for(int i = 0; i<buf_len; i++){
            printf("%x ", buf[i]);
        }

        return STS_send_OK;
    }
}



/*
    Here we wait for STS0 to arrive.
    If there is a timeout or a wrong tag or 
    an invalid signature,just stay in this state.
    Otherwise, the packet is correct: Use the data
    to perform computations and go to state STS_send_1
    to send this new data
*/
State STS_receive_0_fct(uint8_t* buf, Memory* mem){
    
    // here we wait for STS1 to arrive

    //if there is a timeout or a wrong tag or an invalid signature
    return STS_send_1;
}
/*
    Here we wait for STS1 to arrive.
    If there is a timeout or a wrong tag or 
    an invalid signature, go back to STS_send_0.
    Otherwise, the packet is correct. Use the data
    to perform computations and go to state STS_send_2
    to send this new data
*/
State STS_receive_1_fct(uint8_t* buf, Memory* mem){
    
    // here we wait for STS1 to arrive

    //if there is a timeout or a wrong tag or an invalid signature
    return STS_send_2;
}

/*
    Here we wait for STS2 to arrive.
    If there is a timeout or a wrong tag or 
    an invalid signature, go back to STS_receive_0.
    Otherwise, the packet is correct. Use the data
    to perform computations and go to state STS_drone_complete  
*/
State STS_receive_2_fct(uint8_t* buf, Memory* mem){
    
    // here we wait for STS1 to arrive

    //if there is a timeout or a wrong tag or an invalid signature
    return STS_drone_completed;
}

State STS_send_OK_fct(uint8_t* buf, Memory* mem){
    printf("\n//////\nState STS_send_OK \n");

    //prepare variables to store data
    uint16_t buf_len;
    uint16_t rcv_buf_len;
    WORD_LEN rcv_data_len;
    WORD_ID rcv_id;
    WORD_TAG rcv_tag;
    uint8_t rcv_data[MAX_DATA_LENGTH];

    // initialize buffer (otherwise it contains precedent data)
    for(int i = 0; i<MAX_TRANSFER_LENGTH; i++){
        buf[i] = 0;
    }

    //receive message
    rcv_buf_len = receive_message(buf);
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, rcv_buf_len);

    // print received message
    printf("Received STS 2: \n");
    printf("\t tag = %d\n", rcv_tag);
    printf("\t len = %d\n", rcv_data_len);
    printf("\t id  = %d\n", rcv_id);
    printf("\t data = ");
	for(int i = 0; i<rcv_data_len; i++){
		printf("%x ", rcv_data[i]);
	}
    printf("\n");

    if(rcv_tag != TAG_STS_2){
        printf("Invalid tag due to timeout or error, return in first state\n");
        return STS_send_1;
    }
    else{
        printf("Valid tag, verify STS_2\n");
        
        //verify STS_2
        
        uint8_t valid_STS2 = 1;
        
        valid_STS2 = verify_STS_2(rcv_data, mem);

        if(valid_STS2 == 0){
            printf("STS_2 invalid, return in first state\n");
            return STS_send_1;
        }

        else{

            printf("Valid STS_2, send next message\n");
           
            // compute new message
            
            WORD_LEN len = 1;                  // making the buf
            getTLV(buf, &buf_len, TAG_STS_OK, len, mem->receiverID, "ok");

            // send message
            if(send_message(buf, buf_len)==0) {
                close_sockets();
                printf("Send failed at message STS2 \n");
                return 0;
            }
            
            // print sent message
            printf("Sent message OK");
            for(int i = 0; i<buf_len; i++){
                printf("%x ", buf[i]);
            }

            return STS_drone_completed;

        }
    }
}
State STS_receive_OK_fct(uint8_t* buf,  Memory* mem){
    printf("\n//////\nState STS_receive_OK \n");

    //prepare variables to store data
    uint16_t rcv_buf_len;
    WORD_LEN rcv_data_len;
    WORD_ID rcv_id;
    WORD_TAG rcv_tag;
    uint8_t rcv_data[MAX_DATA_LENGTH];

    // initialize buffer (otherwise it contains precedent data)
    for(int i = 0; i<MAX_TRANSFER_LENGTH; i++){
        buf[i] = 0;
    }

    //receive message
    rcv_buf_len = receive_message(buf);
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, rcv_buf_len);

    // print received message
    printf("Received OK: \n");
    printf("\t tag = %d\n", rcv_tag);
    printf("\t len = %d\n", rcv_data_len);
    printf("\t id  = %d\n", rcv_id);
    printf("\t data = ");
	for(int i = 0; i<rcv_data_len; i++){
		printf("%x ", rcv_data[i]);
	}
    printf("\n");

    if(rcv_tag != TAG_STS_OK){
        printf("Invalid tag due to timeout or error, return in first state\n");
        return STS_send_0;
    }
    else{
        printf("Valid tag, STS protocol finished\n");
        return STS_CC_completed;
    }
}    


State STS_CC_completed_fct(){
    printf("\n/////\nState STS_CC_completed \n");
    return State_Exit;
}

State STS_drone_completed_fct(){
    printf("\n/////\nState STS_drone_completed \n");
    return State_Exit;
}



void make_STS_0_data(uint8_t *data, Memory* mem){     //STS_0_data = pointc.x||pointc.y 

    //to make STS_0, a random number c is generated, this is multiplied with the generator G, and results in pointc 
    //point1 = (pointc.x, pointc.y) = c*G, data of STS_0 is point1.x concatenated with pointc.y, pointc.x and pointc.y both have 256 bits, 
    //              thus the data has 512 bits 

	/*
	 * sajetan: 22/April- this will need some modification as we will not send both X and Y coordinates but only X coordinates of the effine.
	 * Below we have generated a random number for computation, this is assumed to be the private key but this code should be moved \
	 * as part of precomputation.
	 * */

   	uint32_t bits_rng = 256;        //initialization
	uint8_t c_8[SIZE] = {0};
    WORD c[SIZE] = {0};
    WORD i;
    p256_integer c_struct = {0};
	p256_affine pointc = {0};

	p256_integer cc_secret_key = {0};
	p256_affine cc_public_key={0};

    //generating random c
//    EntropyPool pool;           
//	initPool(&pool);
    random(c, bits_rng, &mem->pool);
    convertArray16toArray8(c_8, c);
    for(i = 0; i<=c_8[0];i++){
    	cc_secret_key.word[i] = c_8[i];
    	mem->SK[i]=c_8[i];
    }
	
    printf("in the memory----");print_hex_type(mem->SK,8);
    print_hex_type(cc_secret_key.word,8);
    //we should store G as a p256_affine structure in the memory
    WORD G_x[SIZE] = {0};           
    WORD G_y[SIZE] = {0};
    p256_affine G = {0};
//    convert(G_x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
//    convert(G_y,  "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
//    copyWord(G.x, G_x);
//	copyWord(G.y, G_y);

    copyArrayWithSize(G.x,mem->GX); //copying from memory
    copyArrayWithSize(G.y,mem->GY); //copying from memory
//    copyArrayWithSize(N.word,mem->N); //copying from memory


    //pointc = c*G
    pointScalarMultAffine(&cc_public_key, &G, cc_secret_key);

    //converting to uint8_t arrays
    uint8_t data1[32+1] = {0}; //sajetan:manage this better to handle the damn array size
    convertArray16toArray8(data1, cc_public_key.x);
    uint8_t data2[32+1] = {0}; //sajetan:manage this better to handle the damn array size
    convertArray16toArray8(data2, cc_public_key.y);

    copyArrayWithSize(mem->PKCX,cc_public_key.x);
	copyArrayWithSize(mem->PKCY,cc_public_key.y);


    printf("control center public keyx----- ");print_hex_type(cc_public_key.x,16);
    printf("control center public keyy----- ");print_hex_type(cc_public_key.y,16);


    //concatenating point1.x and point1.y , in the next step while constructing data, make sure the concatetanation is similar to the state1
    for(i=0;i<32;i++){
        data[i] = data1[i+1];
        data[32+i] = data2[i+1];
    }
//    printf("\nfinal data-----");print_num_size_type(data,64,8);
//
//    printf("data %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x  ",data[0],data[1],data[2],data[3],data[4], data[5],data[6],data[7],data[8],data[9]);
//    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x ",data[10],data[11],data[12],data[13],data[14], data[15],data[16],data[17],data[18],data[19]);
//    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x",data[20],data[21],data[22],data[23],data[24], data[25],data[26],data[27],data[28],data[29]);
//    printf(" %04x %04x %04x  \n",data[30],data[31],data[32]);
//
//    printf("data1 %04x %04x %04x %04x %04x %04x %04x %04x  \n",data1[0],data1[1],data1[2],data1[3],data1[4], data1[5],data1[6],data1[7],data1[8],data1[9]);
//    printf("data2 tpkc %04x %04x %04x %04x %04x %04x %04x %04x  \n",data2[0],data2[1],data2[2],data2[3],data2[4], data2[5],data2[6],data2[7],data2[8],data2[9]);

}


void make_STS_1_data(uint8_t *data, uint8_t *rcv_data, Memory* mem){   //STS_1_data = pointd.x||pointc.x||Encryption[signature(pointd.x||pointc.x)]
                                                          //rcv_data = STS_0_data = pointc.x||pointc.y  
    //generate random d                                   
   	uint32_t bits_rng = 256;        //initialization
	p256_affine G = {0};
	p256_integer N={0};

   	p256_integer drone_secret_key = {0};
    WORD i;
    WORD generate_random_key[SIZE] = {0};
    uint8_t generate_random_key_8[SIZE] = {0};
	p256_affine drone_public_key = {0};
	WORD public_key_dronex_ccx[64] = {0};
	WORD signed_message[64] = {0};
    uint8_t signed_message_8[SIZE] = {0};


	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint8_t vciphertext[MAX_MSG_SIZE]={0};
//    WORD encrypted_message[2048] = {0};
//	WORD encrypted_message[2048] = {0};

	p256_affine cc_public_key = {0};
	p256_affine session_key_affine = {0};
	uint8_t session_key_8[CHACHA_KEY_LENGTH] = {0};
	WORD drone_secret_key_copy[SIZE]={0};
	uint8_t drone_secret_key_8[CHACHA_KEY_LENGTH] = {0};

	p256_integer session_key = {0};
	uint8_t rcv_mac_tag[MAC_TAG_LENGTH] = {0};

    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};

    //we should store G as a p256_affine structure in the memory
//    convert(G.x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
//    convert(G.y, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
//    convert(N.word, "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
//    printf("----coordinates \n");
//    print_num(G.x);
//    print_num(G.y);
//    print_num(N.word);

    copyArrayWithSize(G.x,mem->GX); //copying from memory
    copyArrayWithSize(G.y,mem->GY); //copying from memory
    copyArrayWithSize(N.word,mem->N); //copying from memory


	//generating random c
    EntropyPool pool; //change this wrt the memory function
	initPool(&pool);
    random(generate_random_key, bits_rng, &pool);
    print_hex_type(generate_random_key,16);
    random(generate_random_key, bits_rng, &pool);
    print_hex_type(generate_random_key,16);



    copyArrayWithSize(drone_secret_key_copy,generate_random_key);
    convertArray16toArray8(generate_random_key_8, generate_random_key);
    P(1)

    for(i = 0; i<=generate_random_key_8[0];i++){
    	drone_secret_key.word[i]=generate_random_key_8[i];

    }


//    print_hex_type(drone_secret_key.word,8);

//    print_num_size(generate_random_key,33);
//    print_num_size(generate_random_key_8,33);
//    printf("inside --- make_STS_1_data \n  lol \n");

//    print_hex_type(drone_secret_key.word,8);

    /*---------------------------------compute pointd = d*G--------------------------------------------------*/

//    printf("key workd----------");print_hex_type(drone_secret_key.word,8);
    printf("drone secret key ------- ");print_hex_type(drone_secret_key.word,8);
    //pointc = c*G
    pointScalarMultAffine(&drone_public_key, &G, drone_secret_key);

//    printf("----coordinates checking again\n");
//        print_num(G.x);
//        print_num(G.y);
//        print_num(N.word);


    printf("drone public key \n");
    print_num(drone_public_key.x);
    print_num(drone_public_key.y);

    printf("\nmake_STS_1_data drone public keyx----- ");print_hex_type(drone_public_key.x,16);
    printf("make_STS_1_data drone public keyy----- ");print_hex_type(drone_public_key.y,16);
    copyArrayWithSize(mem->PKDX,drone_public_key.x); //copy to memory
 	copyArrayWithSize(mem->PKDY,drone_public_key.y); //copy to memory


    uint16_t cc_pk[64]={0};
    convertArray8toArray16withoutLen(cc_pk, rcv_data,64);

    printf("received cc public key data %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x  ",rcv_data[0],rcv_data[1],rcv_data[2],rcv_data[3],rcv_data[4], rcv_data[5],rcv_data[6],rcv_data[7],rcv_data[8],rcv_data[9]);
    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x ",rcv_data[10],rcv_data[11],rcv_data[12],rcv_data[13],rcv_data[14], rcv_data[15],rcv_data[16],data[17],rcv_data[18],rcv_data[19]);
    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x",rcv_data[20],rcv_data[21],rcv_data[22],rcv_data[23],rcv_data[24], rcv_data[25],rcv_data[26],data[27],rcv_data[28],rcv_data[29]);
    printf(" %04x %04x %04x  \n",rcv_data[30],rcv_data[31],rcv_data[32]);

    cc_public_key.x[0]=16;
    cc_public_key.y[0]=16;


    for(i=0;i<32;i++){
    	cc_public_key.x[i+1] = cc_pk[i];
    	cc_public_key.y[i+1] = cc_pk[i+16];
    }
    copyArrayWithSize(mem->PKCX,cc_public_key.x); //copy to memory
   	copyArrayWithSize(mem->PKCY,cc_public_key.y); //copy to memory



    //printf("---recvd data - pkc 16\n");print_num(pkc);
    printf("make_STS_1_data [control center public keyx]----- ");print_hex_type(cc_public_key.x,16);
    printf(" %04x %04x %04x  \n",cc_public_key.x[0],cc_public_key.x[1],cc_public_key.x[2]);
    printf("make_STS_1_data [control center public keyy]----- ");print_hex_type(cc_public_key.y,16);


    /*--------------------------------- compute session key = (x,y) = d*pointc ------------------------------------*/
    pointScalarMultAffine(&session_key_affine, &cc_public_key, drone_secret_key);

    printf("make_STS_1_data session_key x affine----- ");print_hex_type(session_key_affine.x,16);
    printf("make_STS_1_data session_key y affine----- ");print_hex_type(session_key_affine.y,16);

    printf("data %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x  ",session_key_affine.x[0],session_key_affine.x[1],session_key_affine.x[2],session_key_affine.x[3],session_key_affine.x[4], session_key_affine.x[5],session_key_affine.x[6],session_key_affine.x[7],session_key_affine.x[8],session_key_affine.x[9]);
    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x ",session_key_affine.x[10],session_key_affine.x[11],session_key_affine.x[12],session_key_affine.x[13],session_key_affine.x[14], session_key_affine.x[15],session_key_affine.x[16],data[17],session_key_affine.x[18],session_key_affine.x[19]);
    printf(" %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x",session_key_affine.x[20],session_key_affine.x[21],session_key_affine.x[22],session_key_affine.x[23],session_key_affine.x[24], session_key_affine.x[25],session_key_affine.x[26],data[27],session_key_affine.x[28],session_key_affine.x[29]);
    printf(" %04x %04x %04x  \n",session_key_affine.x[30],session_key_affine.x[31],session_key_affine.x[32]);

    
    //compute session key by taking hash of session key x coordinate
    hash(session_key.word,session_key_affine.x,256);
    printf("\nmake_STS_1_data SESSION KEY----- ");print_hex_type(session_key.word,16);


    //
    WORD data_for_sign[32]={0};

    for(i=1;i<=16;i++){
    	public_key_dronex_ccx[i]    = drone_public_key.x[i];
    	public_key_dronex_ccx[i+16] = cc_public_key.x[i];
    }
    public_key_dronex_ccx[0]=64/sizeof(WORD);


    printf("\nmake_STS_1_data concatenated [drone+cc key]----- ");print_hex_type(public_key_dronex_ccx,16);
    print_num(public_key_dronex_ccx);



    /*---------------------------------sign (pointd.x||pointc.x) with the private key of the drone --------------------*/
    //printf("drone secret key ------- ");print_hex_type(drone_secret_key_8,8);// print_num(drone_secret_key_8);

    printf("gx,gy, n----coordinates again again\n");
        print_num(G.x);P(1)
        print_num(G.y);P(2)
        print_num(N.word);P(3)


	signature_gen(signed_message,drone_secret_key_copy,public_key_dronex_ccx, N.word, G.x, G.y);
    //signature_gen(signed_message,drone_secret_key.word,public_key_dronex_ccx, N.word, G.x, G.y);
    printf("\n make_STS_1_data SIGNATURE GENERATED---- ");print_num(signed_message);

    convertArray16toArray8withoutLen(signed_message_8, signed_message);

    convertArray16toArray8withoutLen(session_key_8, session_key.word);

    memcpy(mem->SESSION_KEY,session_key_8, CHACHA_KEY_LENGTH); //copy to memory

    printf("make_STS_1_data signed message_8- ");print_num_type_length(signed_message_8,signed_message[0]*2,8);
    printf("make_STS_1_data session_key_8- ");print_num_type_length(session_key_8,32,8);

    char2byte(nonce, "4041424344454647"); // should be changed to random number

    /*---------------------------------encrypt the signature with the session key k //need to manage lengths of plaintext here! --------------------------*/
	aead_chacha20_poly1305(mac_tag,ciphertext, session_key_8, 32, nonce, signed_message_8, signed_message[0]*2, "50515253c0c1c2c3c4c5c6c7");

	printf("make_STS_1_data MAC GENERATED- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("make_STS_1_data CIPHERTEXT GENERATED- ");print_num_type_length(ciphertext,signed_message[0]*2,8);

	//testing by deciphering the ciphertext and checking if it mataches signed message
//	aead_chacha20_poly1305(rcv_mac_tag,vciphertext, session_key_8, 32, nonce, ciphertext, signed_message[0]*2, "50515253c0c1c2c3c4c5c6c7");
	//encrypt_init(vciphertext,session_key_8, 32, nonce, ciphertext,64, 0);
//	printf("mac- ");print_num_type_length(rcv_mac_tag,MAC_TAG_LENGTH,8);
//	printf("-\n--vciphertext- ");print_num_type_length(vciphertext,signed_message[0]*2,8);



	/*---------------------------------concatenate everything in data -----------------------------------------------*/

    //concatenating pointd.x and pointd.y

	//Number of times in concatenation px||py || nonce || ciphertext||mac  so 5 will also send it's lengths of 16bits
	uint8_t total_data=6; //5 for 5 lengths. One byte for px,py, nonce, mac, ciphertext (with an assumption that ciphertext will always be 64bytes) and one byte for this tag; sajetan : make this look clear
	data[0]=total_data; //indicates number of concatenated fields

	data[1]=CHACHA_NONCE_LENGTH;

	data[2]=drone_public_key.x[0]*2;

	data[3]=drone_public_key.y[0]*2;

	data[4]=signed_message[0]*2;

	data[5]=MAC_TAG_LENGTH;

//	printf("length of public key %d \n",data[1]);

	for(i=0;i<data[1];i++){
		data[total_data+i] = nonce[i];
	}


	for(i=0;i<data[2]/2;i++){
		data[total_data+data[1]+i*2] = drone_public_key.x[i+1];
        data[total_data+data[1]+i*2+1] = drone_public_key.x[i+1] >> 8 ;
	//	printf("length of public key %d %02x %d %02x [%d]\n",total_data+i*2, data[total_data+i*2],total_data+i*2+1,data[total_data+i*2+1],i);
	}

	for(i=0;i<data[3]/2;i++){
		data[total_data+data[1]+data[2]+i*2] = drone_public_key.y[i+1];
        data[total_data+data[1]+data[2]+i*2+1] = drone_public_key.y[i+1] >> 8 ;
    }


	for(i=0;i<(data[4]);i++){
		data[total_data+data[1]+data[2]+data[3]+i] = ciphertext[i];
    }

	for(i=0;i<data[5];i++){
		data[total_data+data[1]+data[2]+data[3]+data[4]+i] = mac_tag[i];
    }

	printf("-\n make_STS_1_data concatenated data- length [ %d ] \n to be sent: ", total_data+data[1]+data[2]+data[3]+data[4]+data[5]);print_num_type_length(data,total_data+data[1]+data[2]+data[3]+data[4]+data[5],8);


}

void make_STS_2_data(uint8_t *data, uint8_t *recv_data, Memory* mem){   //STS_2_data = Encryption[signature(pointc.x||pointd.x)]
                                                          //rcv_data = STS_1_data = pointd.x||pointd.y||Encryption[signature(pointd.x||pointc.x)]
    //generate random d
   	uint32_t bits_rng = 256;        //initialization

   	p256_integer drone_secret_key = {0};
    WORD i;
    WORD generate_random_key[SIZE] = {0};
    uint8_t generate_random_key_8[SIZE] = {0};
	p256_affine drone_public_key = {0};
	WORD public_key_ccx_dronex[64] = {0};
	WORD signed_message[64] = {0};
    uint8_t signed_message_8[SIZE] = {0};


	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint8_t vciphertext[MAX_MSG_SIZE]={0};

	p256_affine cc_public_key = {0};
	p256_affine session_key_affine = {0};
	uint8_t session_key_8[CHACHA_KEY_LENGTH] = {0};
	p256_integer session_key = {0};
	p256_affine G = {0};
	p256_integer N={0};
	uint8_t rcv_mac_tag[MAC_TAG_LENGTH] = {0};

    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};


//    //need to make this available from outside
//    convert(G.x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
//    convert(G.y, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
//    convert(N.word, "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
//    printf("----coordinates \n");print_num(G.x);
//    print_num(G.y);
//    print_num(N.word);

    copyArrayWithSize(G.x,mem->GX); //copying from memory
    copyArrayWithSize(G.y,mem->GY); //copying from memory
    copyArrayWithSize(N.word,mem->N); //copying from memory



	//compute (x,y) = c*pointd -- taken from memory

    //compute session key k = hash(x) -- taken from memory
    //sign (pointc.x||pointd.x) with the private key of the control center -- taken from memory

    for(i=1;i<=16;i++){
    	public_key_ccx_dronex[i]    = mem->PKCX[i]; // cc_public_key.x[i];
    	public_key_ccx_dronex[i+16] = mem->PKDX[i]; //drone_public_key.x[i];
    }

    printf("make_STS_2_data PKC x----- ");print_hex_type(mem->PKCX,16);
    printf("make_STS_2_data PKD y----- ");print_hex_type(mem->PKDX,16);
    printf("make_STS_2_data Secret key y----- ");print_hex_type(mem->SK,16);
    //printf("Session key y----- ");print_hex_type(mem->SESSION_KEY,16);
    printf("make_STS_2_data session_key_8- ");print_num_type_length(mem->SESSION_KEY,32,8);

    public_key_ccx_dronex[0]=64/sizeof(WORD);
    printf("\n make_STS_2_data concatenated [cc+drone key] ----- ");print_hex_type(public_key_ccx_dronex,16);
    print_num(public_key_ccx_dronex);

    signature_gen(signed_message, mem->SK, public_key_ccx_dronex, N.word, G.x, G.y);
    printf("\nmake_STS_2_data SIGNATURE OUTPUT---- ");print_num(signed_message);
    convertArray16toArray8withoutLen(signed_message_8, signed_message);
    P(1);
    char2byte(nonce, "4041424344454647"); // should be changed to random number using this for testing only
    P(2);
    //encrypt the signature with the session key k
   	aead_chacha20_poly1305(mac_tag,ciphertext, mem->SESSION_KEY, 32, nonce, signed_message_8, signed_message[0]*2, "50515253c0c1c2c3c4c5c6c7");
   	P(3);
   	printf("make_STS_2_data MAC GENERATED- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("make_STS_2_data CIPHERTEXT GENERATED- ");print_num_type_length(ciphertext,signed_message[0]*2,8);



    // data = encryption of this signature

	/*---------------------------------concatenate everything in data -----------------------------------------------*/

    //concatenating pointd.x and pointd.y

	//Number of times in concatenation nonce || ciphertext||mac  so 3 will also send it's lengths of 16bits
	uint8_t total_data=4; //3 for 3 lengths. One byte for nonce, mac, ciphertext (with an assumption that ciphertext will always be 64bytes) and one byte for this tag; sajetan : make this look clear
	data[0]=total_data; //indicates number of concatenated fields

	data[1]=CHACHA_NONCE_LENGTH;

	data[2]=signed_message[0]*2;

	data[3]=MAC_TAG_LENGTH;

	//printf("length of public key %d \n",data[1]);

	for(i=0;i<data[1];i++){
		data[total_data+i] = nonce[i];
	}

	for(i=0;i<(data[4]);i++){
		data[total_data+data[1]+i] = ciphertext[i];
    }

	for(i=0;i<data[5];i++){
		data[total_data+data[1]+data[2]+i] = mac_tag[i];
    }

	printf("-\n-- make_STS_2_data concatenated data after encryption- length [ %d ] \n to be sent:  ", total_data+data[1]+data[2]+data[3]);print_num_type_length(data,total_data+data[1]+data[2]+data[3],8);

}


uint8_t verify_STS_1(uint8_t *recv_data, Memory* mem){    //rcv_data = STS_1_data = pointd.x||pointd.y||nonce||Encryption[signature(pointd.x||pointc.x)]||MAC
    printf("in verify the memory----");print_hex_type(mem->SK,8);

	//clean this up later

	p256_affine G = {0};
	p256_affine drone_public_key = {0};
	p256_affine cc_public_key = {0};
	p256_affine session_key_affine = {0};
	p256_integer drone_secret_key = {0};
   	p256_integer cc_secret_key = {0};
	p256_integer session_key = {0};
	p256_integer N={0};

	uint32_t message_length=0;
	WORD i;
    WORD generate_random_key[SIZE] = {0};
	WORD public_key_dronex_ccx[64] = {0};
	WORD signed_message[64] = {0};
    WORD plaintext_word[MAX_MSG_SIZE]={0};

	uint8_t session_key_8[CHACHA_KEY_LENGTH] = {0};
	uint8_t signed_message_8[SIZE] = {0};
	uint8_t rcv_mac_tag[MAC_TAG_LENGTH] = {0};
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint8_t plaintext[MAX_MSG_SIZE]={0};
    uint8_t generate_random_key_8[SIZE] = {0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};

//    //we should store G as a p256_affine structure in the memory
//    convert(G.x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
//    convert(G.y, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
//    convert(N.word, "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
//    printf("----coordinates \n");print_num(G.x);
//    print_num(G.y);
//    print_num(N.word);

    copyArrayWithSize(G.x,mem->GX); //copying from memory
    copyArrayWithSize(G.y,mem->GY); //copying from memory
    copyArrayWithSize(N.word,mem->N); //copying from memory


   // if (recv_data[0]!=6) return; //number of tags should be 5
    WORD total_data =recv_data[0];

	for(i=0;i<recv_data[1];i++){
		nonce[i] = recv_data[total_data+i];
	}

	drone_public_key.x[0]=16;
	drone_public_key.y[0]=16;


	for(i=0;i<recv_data[2]/2;i++){
		//drone_public_key.x[i+1] = data[total_data+recv_data[1]+i*2] ;
		drone_public_key.x[i+1] = ((recv_data[total_data+recv_data[1]+i*2+1]<< 8) | recv_data[total_data+recv_data[1]+i*2]);
	//	printf("length of public key %d %02x %d %02x [%d]\n",total_data+i*2, data[total_data+i*2],total_data+i*2+1,data[total_data+i*2+1],i);
	}

	for(i=0;i<recv_data[3]/2;i++){
		drone_public_key.y[i+1] = ((recv_data[total_data+recv_data[1]+recv_data[2]+i*2+1]<< 8) | recv_data[total_data+recv_data[1]+recv_data[2]+i*2]);
		//data[total_data+data[1]+data[2]+i*2] = drone_public_key.y[i+1];
        //data[total_data+data[1]+data[2]+i*2+1] = drone_public_key.y[i+1] >> 8 ;
    }

	message_length=recv_data[4];
	for(i=0;i<(recv_data[4]);i++){
		ciphertext[i]=recv_data[total_data+recv_data[1]+recv_data[2]+recv_data[3]+i];
    }

	for(i=0;i<recv_data[5];i++){
		rcv_mac_tag[i]=recv_data[total_data+recv_data[1]+recv_data[2]+recv_data[3]+recv_data[4]+i];
    }

    printf("\ndrone public keyx----- ");print_hex_type(drone_public_key.x,16);
    printf("drone public keyy----- ");print_hex_type(drone_public_key.y,16);

	printf("mac- ");print_num_type_length(rcv_mac_tag,MAC_TAG_LENGTH,8);
	printf("ciphertext- ");print_num_type_length(ciphertext,recv_data[4],8);


//	cc_secret_key
    copyArrayWithSize(cc_secret_key.word, mem->SK);
    copyArrayWithSize(cc_public_key.x,mem->PKCX);
	copyArrayWithSize(cc_public_key.y,mem->PKCY);

    copyArrayWithSize(mem->PKDX,drone_public_key.x);
	copyArrayWithSize(mem->PKDY,drone_public_key.y);

	/* lets now extract all the components from rcv_data */

    //compute (x,y) = c*pointd
    pointScalarMultAffine(&session_key_affine, &drone_public_key, cc_secret_key);
    printf("session_key x affine----- ");print_hex_type(session_key_affine.x,16);
    printf("session_key y affine----- ");print_hex_type(session_key_affine.y,16);

    //compute session key k = hash(x)
    //compute session key by taking hash of session key x coordinate
    hash(session_key.word,session_key_affine.x,256);
    printf("\n verify_STS_1SESSION KEY----- ");print_hex_type(session_key.word,16);

    convertArray16toArray8withoutLen(session_key_8, session_key.word);
    printf("verify_STS_1 session_key_8 in bytes- ");print_num_type_length(session_key_8,32,8);

    memcpy(mem->SESSION_KEY,session_key_8, CHACHA_KEY_LENGTH); //copy to memory
    printf("verify_STS_1 copy SESSION KEY - ");print_num_type_length(mem->SESSION_KEY,32,8);


    //verify_mac_aead_chacha20_poly1305(rcv_mac_tag, session_key_8, 32, nonce, ciphertext, message_length, "50515253c0c1c2c3c4c5c6c7"); //this is not working

   // verify_mac_aead_chacha20_poly1305(rcv_mac_tag, session_key_8, 32, nonce, ciphertext, message_length, "50515253c0c1c2c3c4c5c6c7");
    //decrypt the signature with k
//    aead_chacha20_poly1305(mac_tag,plaintext, session_key_8, 32, nonce, ciphertext, message_length, "50515253c0c1c2c3c4c5c6c7");
    encrypt_init(plaintext,session_key_8, 32, nonce, ciphertext,64, 0);
//	printf("mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("verify_STS_1 Plaintext sign(pkdx || pkcx) ");print_num_type_length(plaintext,message_length,8);

    for(i=1;i<=16;i++){
    	public_key_dronex_ccx[i]    = drone_public_key.x[i];
    	public_key_dronex_ccx[i+16] = cc_public_key.x[i];
    }
    public_key_dronex_ccx[0]=64/sizeof(WORD);
    printf("\n verify_STS_1 concatenated [drone+cc key]----- ");print_hex_type(public_key_dronex_ccx,16);
    print_num(public_key_dronex_ccx);

    //signature message plaintext to verify
    for(i=0;i<message_length;i++){
    	plaintext_word[i+1]    = ((plaintext[i*2+1]<< 8) |plaintext[i*2]);
    }
    plaintext_word[0]=message_length/2;

	printf("\n----BEFORE [3] signature coordinates originall\n");
	print_num(G.x);
	print_num(G.y);
	print_num(N.word);


//    convertArray8toArray16withoutLen(plaintext_word, plaintext,message_length);
    P(1);
    //verify the signature with the public key of the drone, if signature valid --> STS_1 valid
	WORD v=sig_ver(plaintext_word, N.word, public_key_dronex_ccx, G.x, G.y, drone_public_key.x,drone_public_key.y);
	P(2);
	printf("--verify_STS_1-SAJETAN---SIGNATURE VERIFICATION ------ [ %d ] \n", v);

}

uint8_t verify_STS_2(uint8_t *recv_data, Memory* mem){    //rcv_data = STS_2_data = Encryption[signature(pointc.x||pointd.x)]

	//clean this up later by removing unwanted items

   	p256_integer drone_secret_key = {0};
   	p256_integer cc_secret_key = {0};
    WORD i;

	WORD public_key_ccx_dronex[64] = {0};
	WORD signed_message[64] = {0};
    uint8_t signed_message_8[SIZE] = {0};

	uint8_t rcv_mac_tag[MAC_TAG_LENGTH] = {0};
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};

    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint8_t plaintext[MAX_MSG_SIZE]={0};
    uint32_t message_length=0;

	p256_affine cc_public_key = {0};
	p256_affine session_key_affine = {0};
	uint8_t session_key_8[CHACHA_KEY_LENGTH] = {0};
	p256_integer session_key = {0};
	p256_affine G = {0};
	p256_integer N={0};

    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};

    copyArrayWithSize(G.x,mem->GX); //copying from memory
    copyArrayWithSize(G.y,mem->GY); //copying from memory
    copyArrayWithSize(N.word,mem->N); //copying from memory


   // if (recv_data[0]!=4) return; //number of tags should be 4
    WORD total_data =recv_data[0];

	for(i=0;i<recv_data[1];i++){
		nonce[i] = recv_data[total_data+i];
	}

	message_length=recv_data[2];
	for(i=0;i<(recv_data[2]);i++){
		ciphertext[i]=recv_data[total_data+recv_data[1]+i];
    }

	for(i=0;i<recv_data[3];i++){
		rcv_mac_tag[i]=recv_data[total_data+recv_data[1]+recv_data[2]+i];
    }

	printf("verify_STS_2 RECV MAC- ");print_num_type_length(rcv_mac_tag,MAC_TAG_LENGTH,8);
	printf("verify_STS_2 RECV CIPHERTEXT- ");print_num_type_length(ciphertext,recv_data[4],8);


    //compute session key k = hash(x)
    //compute session key by taking hash of session key x coordinate
    printf("verify_STS_2 SESSION KEY");print_num_type_length(mem->SESSION_KEY,32,8);

    //verify_mac_aead_chacha20_poly1305(rcv_mac_tag, session_key_8, 32, nonce, ciphertext, message_length, "50515253c0c1c2c3c4c5c6c7"); //this is not working

    //decrypt the signature with k
//    aead_chacha20_poly1305(mac_tag,plaintext, session_key_8, 32, nonce, ciphertext, message_length, "50515253c0c1c2c3c4c5c6c7");
    //decrypt the signature with the session key k (which is computed in the make_STS_1_data-function)
    encrypt_init(plaintext,mem->SESSION_KEY, 32, nonce, ciphertext,message_length, 0);
//	printf("mac- ");print_num_type_length(mac_tag,MAC_TAG_LENGTH,8);
	printf("verify_STS_2 PLAINTEXT sign(pkcx || pkdx )- ");print_num_type_length(plaintext,message_length,8);

    for(i=1;i<=16;i++){
    	public_key_ccx_dronex[i]    = mem->PKCX[i];
    	public_key_ccx_dronex[i+16] = mem->PKDX[i];
    }

    public_key_ccx_dronex[0]=64/sizeof(WORD);
    printf("\n verify_STS_2 Concatenated [drone+cc key ]----- ");print_hex_type(public_key_ccx_dronex,16);
    print_num(public_key_ccx_dronex);


    /*---------------- verify the signature with the public key of the control center, if signature valid --> STS_2 valid --------------*/
	WORD v=sig_ver(plaintext, N.word, public_key_ccx_dronex, G.x, G.y, mem->PKCX,mem->PKCY);

	printf("--verify_STS_2--SAJETAN--SIGNATURE VERIFICATION ------ [ %d ] \n", v);



}
