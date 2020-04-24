/*
 * fsm.c
 * 
 *  Created on: April 18, 2020
 *      Author: Ferdinand Hannequart
 */

#include"fsm.h"

void initMemory(Memory* mem, char* publicKey, char* secretKey){

    mem->receiverID = 117;
    initPool(&mem->pool);
    accumulate(&mem->pool, 100);
    convert(mem->pk, publicKey);
    convert(mem->sk, secretKey);
    //convert(mem->G.x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
    //convert(mem->G.y,  "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");

  
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

        //valid_STS1 = verify_STS_1(rcv_data);

        if(valid_STS1 == 0){
            printf("STS_1 invalid, return in first state\n");
            return STS_send_0;
        }

        else{

            printf("Valid STS_1, send next message\n");
           
            // compute new message
            
            uint8_t data[64] = {0}; 

            // make_STS_2_data(data,rcv_data); 

            WORD_LEN len = 64;                  // making the buf
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
        uint8_t data[128] = {0};    
       
        //make_STS_1_data(data, rcv_data);   

        WORD_LEN len = 128;                  // making the buf
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
        
        //valid_STS2 = verify_STS_2(rcv_data);

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

   	uint32_t bits_rng = 256;        //initialization
	uint8_t c_8[SIZE] = {0};
    WORD c[SIZE] = {0};
    WORD i;
    p256_integer c_struct = {0};
	p256_affine pointc = {0};

    //generating random c
    random(c, bits_rng, &mem->pool);

    convertArray16toArray8(c_8, c);
    for(i = 0; i<=c_8[0];i++){
        c_struct.word[i] = c_8[i];
    }
	
    //we should store G as a p256_affine structure in the memory
    
    WORD G_x[SIZE] = {0};           
    WORD G_y[SIZE] = {0};
    p256_affine G = {0};
    convert(G_x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
    convert(G_y,  "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
    copyWord(G.x, G_x);
	copyWord(G.y, G_y);
    

    //pointc = c*G
    pointScalarMultAffine(&pointc, &G, c_struct);       

    //converting to uint8_t arrays
    uint8_t data1[32] = {0};                        
    convertArray16toArray8(data1, pointc.x);
    uint8_t data2[32] = {0};
    convertArray16toArray8(data2, pointc.y);

    //concatenating point1.x and point1.y
    for(i=1;i<=32;i++){
        data[i-1] = data1[i];
    }
    for(i=33;i<=64;i++){
        data[i-1] = data2[i];
    }
}


void make_STS_1_data(uint8_t *data, uint8_t *rcv_data){   //STS_1_data = pointd.x||pointd.y||Encryption[signature(pointd.x||pointc.x)]
                                                          //rcv_data = STS_0_data = pointc.x||pointc.y  
    //generate random d                                   

    //compute pointd = d*G

    //compute (x,y) = d*pointc 
    
    //compute session key k = hash(x)

    //sign (pointd.x||pointd.y) with the private key of the drone

    //encrypt the signature with the session key k
    
    //concatenate everything in data
}

void make_STS_2_data(uint8_t *data, uint8_t *rcv_data){   //STS_2_data = Encryption[signature(pointc.x||pointd.x)]
                                                          //rcv_data = STS_1_data = pointd.x||pointd.y||Encryption[signature(pointd.x||pointc.x)]

    //compute (x,y) = c*pointd 
    
    //compute session key k = hash(x)

    //sign (pointc.x||pointd.x) with the private key of the control center

    //encrypt the signature with the session key k
    
    // data = encryption of this signature
}


uint8_t verify_STS_1(uint8_t *rcv_data){    //rcv_data = STS_1_data = pointd.x||pointd.y||Encryption[signature(pointd.x||pointd.y)]
    //compute (x,y) = c*pointd 
    
    //compute session key k = hash(x)

    //decrypt the signature with k

    //verify the signature with the public key of the drone, if signature valid --> STS_1 valid


}

uint8_t verify_STS_2(uint8_t *rcv_data){    //rcv_data = STS_2_data = Encryption[signature(pointc.x||pointd.x)]

    //decrypt the signature with the session key k (which is computed in the make_STS_1_data-function)

    //verify the signature with the public key of the control center, if signature valid --> STS_2 valid

}
