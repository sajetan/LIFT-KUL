/*
 * fsm.c
 * 
 *  Created on: April 18, 2020
 *      Author: Ferdinand Hannequart
 */

#include"fsm.h"

void initMemory(Memory* mem){

    // entropy pool
    initPool(&mem->pool);
    accumulate(&mem->pool, 100);

    // parameters
    initArray(mem->N,SIZE);
    convert(mem->N,"ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
    initArray(mem->G.x,SIZE);
    initArray(mem->G.y,SIZE);
    convert(mem->G.x, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
    convert(mem->G.y, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
    mem->receiverID = 117;

    //permanent keys
    initArray(mem->drone_SK,SIZE);
    initArray(mem->drone_PK.x,SIZE);
    initArray(mem->drone_PK.y,SIZE);
    initArray(mem->control_SK,SIZE);
    initArray(mem->control_PK.x,SIZE);
    initArray(mem->control_PK.y,SIZE);
    //convert(mem->control_SK, "f257a192dde44227b3568008ff73bcf599a5c45b32ab523b5b21ca582fef5a0a");
    //convert(mem->drone_SK, "bf92c9eb8ff61db0cd3e378b22b3887613afcd1861b11dfbf211d19489f6a08b");
    random(mem->drone_SK, SIZE_EC_KEY, &mem->pool);
    random(mem->control_SK, SIZE_EC_KEY, &mem->pool);
    pointScalarMultAffineWord(&mem->drone_PK, &mem->G, mem->drone_SK);
    pointScalarMultAffineWord(&mem->control_PK, &mem->G, mem->control_SK);

    // session specific
    initArray(mem->drone_secret,SIZE);
    initArray(mem->pointd.x,SIZE);
    initArray(mem->pointd.y,SIZE);
    initArray(mem->control_secret,SIZE);
    initArray(mem->pointc.x,SIZE);
    initArray(mem->pointc.y,SIZE);
    initArray(mem->session_key,SIZE);
    initArray8(mem->session_key8,CHACHA_KEY_LENGTH);

    // communication specific
    initArray8(mem->send_buf,MAX_TRANSFER_LENGTH);
    initArray8(mem->rcv_buf,MAX_TRANSFER_LENGTH);
    initArray8(mem->rcv_data,MAX_DATA_LENGTH);
    mem->send_buf_len = 0;
    mem->rcv_buf_len = 0;

    // debugging
    mem->counter = 0;
    mem->print = 1;

} // end initMemory



/*  Depending on which is the current state, the according packet will be constructed here.
    For the STS-related states, STS message 0, 1 and 2 are constructed here.
    The resulting packet will be stored in the memory, as well as its length. */
void make_packet(Memory* mem, State state){
    
    uint8_t data[MAX_DATA_LENGTH] = {0};
	WORD_LEN len = 0;
    WORD_TAG tag = 0;
    
    // depending on which is the current state, the according packet is constructed
    switch (state){
        case STS_make_0:
            make_STS_0_data(data, mem, &len);    //STS_0_data = pointc.x||pointc.y
            tag = TAG_STS_0;
            break;
        case STS_make_1:
            make_STS_1_data(data, mem, &len);    //STS_1_data = pointd.x||pointc.x||Encryption[signature(pointd.x||pointc.x)]
            tag = TAG_STS_1;
            break;
        case STS_make_2:
            make_STS_2_data(data, mem, &len);    //STS_2_data = Encryption[signature(pointc.x||pointd.x)]
            tag = TAG_STS_2;
            break;
        default:
            printf("problem here: a make function is not defined for this state (%s)\n", __func__);
            printf("tag: %d\n", state);
            assert(0);
            break;
    }
    // assemble the packet (TLV format)
    getTLV(mem->send_buf, &mem->send_buf_len, tag, len, mem->receiverID, data);
}

/*  This function sends the packet over the udp socket */
void send_packet(Memory* mem){
    
    if(send_message(mem->send_buf, mem->send_buf_len)==0) {
		close_sockets();
		printf("Send failed in %s\n", __func__);
		return 0;
	}
    // print sent message
    if(1){
        printf("Sent message:    \n");
        for(int i = 0; i<mem->send_buf_len; i++){
            printf("%x ", mem->send_buf[i]);
        }
    }
}

/*  Here we receive messages and verify their correctness.
    Returns 0 if incorrect message, 1 otherwise.
    Following checks are performed: id, tag and signature(only for STS messages)  */
uint16_t receive_packet(Memory* mem, WORD_TAG expectedTag){
    WORD_LEN rcv_data_len = 0;
    WORD_LEN rcv_buf_len = 0;
    WORD_ID rcv_id = 0;
    WORD_TAG rcv_tag = 0;
    uint8_t rcv_data[MAX_DATA_LENGTH] = {0};
    uint8_t rcv_buf[MAX_TRANSFER_LENGTH] = {0};
    uint16_t valid = 1;

    //receive message
    rcv_buf_len = receive_message(rcv_buf);
    //mem->rcv_buf_len = receive_message(mem->rcv_buf);
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, rcv_buf, rcv_buf_len);
    copyBytes(mem->rcv_data,rcv_data, MAX_DATA_LENGTH);
    mem->rcv_buf_len = rcv_buf_len;

    // print received message
    if(mem->print){
    printf("Received: \n");
    printf("\t tag = %d\n", rcv_tag);
    printf("\t len = %d\n", rcv_data_len);
    printf("\t id  = %d\n", rcv_id);
    printf("\t data = ");
	print_array8(rcv_data, rcv_data_len);
    printf("\n");
    }

    // perform the checks
    if(rcv_tag != expectedTag){
        printf("Invalid tag field (due to timeout or error)\n");
        valid = 0;
    }
    else if(rcv_id != mem->receiverID){
        printf("Invalid id field\n");
        valid = 0;
    }
    else{
        switch (expectedTag){
            case TAG_STS_0:
                valid = verify_STS_0(rcv_data, mem);
                break;
            case TAG_STS_1:
                valid = verify_STS_1(rcv_data, mem);
                break;
            case TAG_STS_2:
                valid = verify_STS_2(rcv_data, mem);
                break;
            default:
                printf("problem here: no verification instructions are defined for this tag %s\n", __func__);
                assert(0);
                break;
            }

        if(valid){
            printf("valid signature\n");
        }
        else{
            printf("invalid signature\n");
        }
    }
    return valid;
}

/* idle state of the control center, its FSM always starts here */
State idle_CC_fct( Memory* mem){
    S()
    mem->counter++;
    printf("\n\n\n\n\n\t\t\t test Nr %d \n\n\n\n", mem->counter);
    //return STS_make_0;
    return key_exchange_CC;
}

/* idle state of the drone, its FSM always starts here */
State idle_drone_fct( Memory* mem){
    S()
    //return STS_receive_0;
    return key_exchange_drone;
}

State STS_make_0_fct( Memory* mem){
    S()
    make_packet(mem, STS_make_0);
    return STS_send_0;
}

State STS_make_1_fct( Memory* mem){
    S()
    make_packet(mem, STS_make_1);
    return STS_send_1;
}

State STS_make_2_fct(Memory* mem){
    S()
    make_packet(mem, STS_make_2);
    return STS_send_2;
}


State STS_send_0_fct(Memory* mem){
    S()
    send_packet(mem);
    return STS_receive_1;
}

State STS_send_1_fct(Memory* mem){
    S()
    send_packet(mem);
    return STS_receive_2;
}

State STS_send_2_fct(Memory* mem){
    S()
    send_packet(mem);
    return STS_receive_OK;
}

/*  Here we wait for STS0 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_0_fct(Memory* mem){
    S()
    uint16_t valid= receive_packet( mem, TAG_STS_0);

    if(!valid){
        // do sth, here I put garbage
        assert(0);
        return STS_make_0;
    } else{
        return STS_make_1;
    }   
}


/*  Here we wait for STS1 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_1_fct(Memory* mem){
    S()
    uint16_t valid = receive_packet( mem, TAG_STS_1);

    if(!valid){
        // do sth, here I put garbage
        assert(0);
        return STS_make_0;
    } else{
        return STS_make_2;
    }
}

/*  Here we wait for STS2 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_2_fct(Memory* mem){
    S()
    uint16_t valid = receive_packet( mem, TAG_STS_2);

    if(!valid){
        // do sth, here I put garbage
        assert(0);
        return STS_make_0;
    } else{
        return STS_send_OK;
    }
}


State STS_CC_completed_fct(){
    S()
    return idle_CC;
    //return State_Exit;
}

State STS_drone_completed_fct(){
    printf("\n/////\nState STS_drone_completed \n");
    return idle_drone;
    //return State_Exit;
}

State key_exchange_CC_fct(Memory* memory){

    uint8_t buffer[MAX_TRANSFER_LENGTH] = {0};
    uint16_t buf_len = 0;
    uint8_t pointx[POINT_LEN] = {0};
    uint8_t pointy[POINT_LEN] = {0};
    p256_affine* pk = &memory->control_PK;
    p256_affine* pk_other = &memory->drone_PK;
    WORD* sk = &memory->control_SK;

    random(sk, SIZE_EC_KEY, &memory->pool);
    pointScalarMultAffineWord(pk, &memory->G, sk);
    word2rawbyte(pointx, pk->x , POINT_LEN);
    word2rawbyte(pointy, pk->y, POINT_LEN );
    for(uint16_t i = 0; i<POINT_LEN; i++){
        buffer[i] = pointx[i];
        buffer[i+POINT_LEN] = pointy[i];
    }
    buf_len = 2*POINT_LEN;

    // send message
    if(send_message(buffer, buf_len)==0) {
        close_sockets();
        printf("Send failed at message STS2 \n");
        return 0;
    }

    buf_len = receive_message(buffer);
    for(uint16_t i = 0; i<POINT_LEN; i++){
        pointx[i] = buffer[i];
        pointy[i] = buffer[i+POINT_LEN];
    }
    rawbyte2word(pk_other->x , pointx, POINT_LEN);
    rawbyte2word(pk_other->y , pointy, POINT_LEN);

    printf("SK control:     : ");       print_hex_type(memory->drone_SK , BIT);
    printf("PK control:     : ");       print_hex_type(memory->control_PK.x , BIT);
    printf("PK control:     : ");       print_hex_type(memory->control_PK.y, BIT);
    printf("PK drone:     : ");       print_hex_type(memory->drone_PK.x , BIT);
    printf("PK drone:     : ");       print_hex_type(memory->drone_PK.y , BIT);


    return STS_make_0;
}

State key_exchange_drone_fct(Memory* memory){

    uint8_t buffer[MAX_TRANSFER_LENGTH] = {0};
    uint16_t buf_len = 0;
    uint8_t pointx[POINT_LEN] = {0};
    uint8_t pointy[POINT_LEN] = {0};
    p256_affine* pk = &memory->drone_PK;
    p256_affine* pk_other = &memory->control_PK;
    WORD* sk = &memory->drone_SK;


    buf_len = receive_message(buffer);
    
    for(uint16_t i = 0; i<POINT_LEN; i++){
        pointx[i] = buffer[i];
        pointy[i] = buffer[i+POINT_LEN];
    }
    rawbyte2word(pk_other->x , pointx, POINT_LEN);
    rawbyte2word(pk_other->y , pointy, POINT_LEN);


    random(sk, SIZE_EC_KEY, &memory->pool);
    pointScalarMultAffineWord(pk, &memory->G, sk);
    word2rawbyte(pointx, pk->x, POINT_LEN );
    word2rawbyte(pointy, pk->y, POINT_LEN );
    for(uint16_t i = 0; i<POINT_LEN; i++){
        buffer[i] = pointx[i];
        buffer[i+POINT_LEN] = pointy[i];
    }
    buf_len = 2*POINT_LEN;

    // send message
    if(send_message(buffer, buf_len)==0) {
        close_sockets();
        printf("Send failed at message STS2 \n");
        return 0;
    }

    printf("SK drone:     : ");       print_hex_type(memory->drone_SK , BIT);
    printf("PK drone:     : ");       print_hex_type(memory->drone_PK.x , BIT);
    printf("PK drone:     : ");       print_hex_type(memory->drone_PK.y , BIT);
    printf("PK control:     : ");       print_hex_type(memory->control_PK.x , BIT);
    printf("PK control:     : ");       print_hex_type(memory->control_PK.y, BIT);

    return STS_receive_0;
}
State STS_send_OK_fct(uint8_t* buf, Memory* mem){
    printf("\n//////\nState STS_send_OK \n");

    //prepare variables to store data
    uint16_t buf_len;

    // initialize buffer (otherwise it contains precedent data)
    for(int i = 0; i<MAX_TRANSFER_LENGTH; i++){
        buf[i] = 0;
    }

//compute new message
            
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














/* STS_0_data = pointc.x||pointc.y  (both coordinates are 256 bits => total of 526 bits of data )*/
void make_STS_0_data(uint8_t *data, Memory* mem, WORD *len){     

    WORD* privateKey        = mem->control_secret; // this is to make life (and writing) easier, privateKey and publicKey are the private and public key stored in the memory
	p256_affine* publicKey  = &mem->pointc;
    WORD i                  = 0;
    uint8_t pointx8[POINT_LEN] = {0};
    uint8_t pointy8[POINT_LEN] = {0};
    
    initArray8(data, MAX_DATA_LENGTH); // to be sure everything is at zero
    initArray(publicKey->x, SIZE); 
    initArray(publicKey->y, SIZE); 
    initArray(privateKey, SIZE); 

    // 1. compute private key
    random(privateKey, SIZE_EC_KEY, &mem->pool);
    //convert(privateKey, "f257a192dde44227b3568008ff73bcf599a5c45b32ab523b5b21ca582fef5a0a");

    // 2. compute public key: pointc = c*G
    pointScalarMultAffineWord(publicKey, &mem->G, privateKey);

    // 3. assemble payload
    word2rawbyte(pointx8, publicKey->x, POINT_LEN);
    word2rawbyte(pointy8, publicKey->y, POINT_LEN);
    for(i=0;i<POINT_LEN;i++){
        data[i]             = pointx8[i];
        data[i+POINT_LEN]   = pointy8[i];
    }
    
    // 4. set data payload length
    *len=2*POINT_LEN;

    // 5. print result
    if(mem->print){
    printf("control center's private key        :"); print_hex_type(privateKey,16);
    printf("control center's public key.x-----  :");print_hex_type(publicKey->x,16);
    printf("control center's public key.y-----  :");print_hex_type(publicKey->y,16);
    }

}
/* STS_1_data = pointd.x||pointc.x||Encryption[signature(pointd.x||pointd.x||pointc.x||pointc.x)] */
void make_STS_1_data(uint8_t *data, Memory* mem, WORD *len){  

    uint8_t signature8[SIG_LEN]         = {0};
    uint8_t pointx8[POINT_LEN]          = {0};
    uint8_t pointy8[POINT_LEN]          = {0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]  = {0};
	uint8_t mac_tag[MAC_TAG_LENGTH]     = {0};
    uint8_t ciphertext[SIG_LEN]         = {0};
    uint16_t start                      = 0;
    
    WORD i                              = 0;
	WORD signature[SIZE]                = {0};
	WORD rand[SIZE]                     = {0};
	WORD toBeSigned[POINT_LEN/BYTE*4+1] = {0};
    WORD*           drone_private_key   = &mem->drone_secret; // private session key of drone
   	p256_affine*    drone_public_key    = &mem->pointd; // public session key of drone
   	p256_affine*    control_public_key  = &mem->pointc; // public session key of drone
    p256_affine     session_key_affine  = {0};

    initArray8(data, MAX_DATA_LENGTH); // to make sure everything is at zero
    initArray(drone_public_key->x, SIZE); 
    initArray(drone_public_key->y, SIZE); 
    initArray(drone_private_key, SIZE); 
    initArray(mem->session_key, SIZE); 
    initArray8(mem->session_key8, CHACHA_KEY_LENGTH); 

    // 1. generate drone's private key, which is a random number
    random(drone_private_key, SIZE_EC_KEY, &mem->pool);
    //convert(drone_private_key, "c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357");

    // 2. generate drone's public key (compute pointd = d*G)
    pointScalarMultAffineWord(drone_public_key, &mem->G, drone_private_key);
    word2rawbyte(pointx8, drone_public_key->x, POINT_LEN);// keep an 8-bit version for putting it in the data packet
    word2rawbyte(pointy8, drone_public_key->y, POINT_LEN);

    // 3. create the signature (mind de fact that i begins at 1 to exclude the length field!)
    for(i=1;i<=POINT_LEN/BYTE;i++){
        toBeSigned[i]                   = drone_public_key->x[i];
        toBeSigned[i+POINT_LEN/BYTE]    = drone_public_key->y[i];
        toBeSigned[i+2*POINT_LEN/BYTE]  = control_public_key->x[i];
        toBeSigned[i+3*POINT_LEN/BYTE]  = control_public_key->y[i];
    }
    toBeSigned[0] = 4*POINT_LEN/BYTE;
    signature_gen(signature,mem->drone_SK,toBeSigned, mem->N, mem->G.x, mem->G.y);

    // 4. compute the session key k = H( (da*Qb).x ) and store it in memory
    pointScalarMultAffineWord(&session_key_affine, control_public_key, mem->drone_secret);  // compute session point
    hash(mem->session_key,session_key_affine.x,SIZEHASH);                                   //compute session key by taking hash of session key x coordinate

    // 5. generate the nonce
    //char2byte(nonce, "4041424344454647"); // should be changed to random number
    random(rand, CHACHA_NONCE_LENGTH*8, &mem->pool);
    word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);

    // 6. encrypt the sigature + compute the mac. Everything must be translated in uint8_t
	word2rawbyte(signature8, signature, SIG_LEN);// keep an 8-bit version for the encryption
    word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH);                   // keep an 8-bit version for the encryption
    aead_chacha20_poly1305(mac_tag,ciphertext, mem->session_key8, 32, nonce, signature8, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //encrypt the signature with the session key k

    // 7. merge everything together
    start = 0;
    for(i=0;i<POINT_LEN;i++){
        data[i]             = pointx8[i];
        data[i+POINT_LEN]   = pointy8[i];
    }
    start += 2*POINT_LEN;
    for(i=0;i<CHACHA_NONCE_LENGTH;i++){
    	data[start + i] = nonce[i];
    }
    start += CHACHA_NONCE_LENGTH;
    for(i=0;i<SIG_LEN;i++){
    	data[start + i] = ciphertext[i];
    }
    start += SIG_LEN;
    for(i=0;i<MAC_TAG_LENGTH;i++){
    	data[start + i] = mac_tag[i];
    }
    start += MAC_TAG_LENGTH;
    *len = start;   // total length of data
    assert(start == 2*POINT_LEN + SIG_LEN + MAC_TAG_LENGTH + CHACHA_NONCE_LENGTH); // small verification

    // 8. print
    if(mem->print){
    printf("cc's secret  (unknown normally stays at 0): ");    print_num(mem->control_secret);
    printf("cc's point x (retrieved): ");    print_num(mem->pointc.x);
    printf("cc's point y (retrieved): ");    print_num(mem->pointc.y);
    printf("\n");
    printf("drone's secret  : ");    print_num(mem->drone_secret);
    printf("drone's point x : ");    print_num(mem->pointd.x);
    printf("drone's point y : ");    print_num(mem->pointd.y);
    printf("\n");
    printf("drone SK        : ");           print_num(mem->drone_SK);
    printf("drone PK.x      : ");           print_num(mem->drone_PK.x);
    printf("drone PK.y      : ");           print_num(mem->drone_PK.y);
    printf("\n");
    printf("[make_STS_1] toBeSigned     : ");    print_hex_type(toBeSigned, BIT);
    printf("[make_STS_1] SECRET KEY     : ");    print_hex_type(mem->drone_SK, BIT);
    printf("[make_STS_1] SIGNATURE      : ");    print_hex_type(signature, BIT);
    printf("[make_STS_1] SESSION KEY :");       print_num(mem->session_key);
    printf("[make_STS_1] NONCE GENERATED:  ");  print_array8(nonce,CHACHA_NONCE_LENGTH);
    printf("[make_STS_1] MAC GENERATED- ");     print_array8(mac_tag,MAC_TAG_LENGTH);
	printf("[make_STS_1] CIPHERTEXT GENERATED- ");print_array8(ciphertext,SIG_LEN);
    printf("STS_1 data      : ");               print_array8(data, *len);
    }

}

/* STS_2_data = Encryption[signature(pointc.x||pointc.y||pointd.x||pointd.y)] */
void make_STS_2_data(uint8_t *data, Memory* mem, WORD *len){   

	p256_affine session_key_affine = {0};
    WORD toBeSigned[SIZE] = {0};
    WORD signature[SIZE] = {0};
    WORD rand[SIZE] = {0};
    uint8_t signature8[SIG_LEN] = {0};
    uint8_t nonce[CHACHA_NONCE_LENGTH] = {0};
    uint16_t i = 0;

    uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t ciphertext[MAX_MSG_SIZE]={0};
    uint16_t start = 0;

    initArray8(data, MAX_DATA_LENGTH); // to be sure


    // 1. compute the signature
    // POINT_LEN is the number of bytes of a point coordinate. Since these are stored in the traditional format with a length field, we chose i+1 to leave it out
    for(i=1;i<=POINT_LEN/BYTE;i++){
        toBeSigned[i]                   = mem->pointc.x[i];
        toBeSigned[i+POINT_LEN/BYTE]    = mem->pointc.y[i];
        toBeSigned[i+2*POINT_LEN/BYTE]  = mem->pointd.x[i];
        toBeSigned[i+3*POINT_LEN/BYTE]  = mem->pointd.y[i];
    }
    toBeSigned[0] = 4*POINT_LEN/BYTE;
    signature_gen(signature,mem->control_SK,toBeSigned, mem->N, mem->G.x, mem->G.y);
    
    // 2. compute session key and store it into memory
    pointScalarMultAffineWord(&session_key_affine, &mem->pointd, mem->control_secret);     // compute session point
    hash(mem->session_key,session_key_affine.x,SIZEHASH);    //compute session key by taking hash of session key x coordinate
    
    // 3. generate the nonce
    //char2byte(nonce, "4041424344454647"); // should be changed to random number
    random(rand, CHACHA_NONCE_LENGTH*8, &mem->pool);
    word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);

    // 4. encrypt the signature + sign it. Everything has to be set to uint8_t
    word2rawbyte(signature8, signature,SIG_LEN);
    word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH); // convert to uint8
	aead_chacha20_poly1305(mac_tag,ciphertext, mem->session_key8, CHACHA_KEY_LENGTH, nonce, signature8, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //encrypt the signature with the session key k

    // 5. merge and adjust length
    start = 0;
    for(i=0;i<CHACHA_NONCE_LENGTH;i++){
    	data[start + i] = nonce[i];
    }
    start += CHACHA_NONCE_LENGTH;
    for(i=0;i<SIG_LEN;i++){
    	data[start + i] = ciphertext[i];
    }
    start +=SIG_LEN;
    for(i=0;i<MAC_TAG_LENGTH;i++){
    	data[start + i] = mac_tag[i];
    }
    start += MAC_TAG_LENGTH;

    *len =  start;
    assert(*len = CHACHA_NONCE_LENGTH + SIG_LEN + MAC_TAG_LENGTH); // check

    // 6. print
    if(mem->print){
    printf("[make_STS_2] toBeSigned     : ");    print_hex_type(toBeSigned, BIT);
    printf("[make_STS_2] SECRET KEY     : ");    print_hex_type(mem->control_SK, BIT);
    printf("[make_STS_2] SIGNATURE      : ");    print_hex_type(signature, BIT);
    printf("[make_STS_2] SESSION KEY    : ");    print_hex_type(mem->session_key, BIT);
    printf("[make_STS_2] NONCE GENERATED: ");    print_array8(nonce, CHACHA_NONCE_LENGTH);
    printf("[make_STS_2] CIPHER         : ");    print_array8(ciphertext, SIG_LEN);
    printf("[make_STS_2] MAC            : ");    print_array8(mac_tag, MAC_TAG_LENGTH);
    }
}
/* rcv_data = STS_0_data = pointc.x||pointc.y */
uint8_t verify_STS_0(uint8_t *rcv_data, Memory* mem){  

    WORD i = 0;
    uint8_t valid = 1;
    uint8_t pointx8[POINT_LEN] = {0};
    uint8_t pointy8[POINT_LEN] = {0};

    initArray(mem->pointc.x, SIZE); 
    initArray(mem->pointc.y, SIZE); 

    // 1.retreive control center's public key from STS_0 message, and store it in memory
    for(i=0;i<POINT_LEN;i++){
    	pointx8[i] = rcv_data[i];
    	pointy8[i] = rcv_data[i+POINT_LEN];
    }
    rawbyte2word(mem->pointc.x,pointx8, POINT_LEN); // transform array of bytes without length field into an array of words with a length field
    rawbyte2word(mem->pointc.y,pointy8, POINT_LEN);

    // 3. prints
    if(mem->print){  
    printf("received cc's point x : ");    print_num(mem->pointc.x);
    printf("received cc's point y : ");    print_num(mem->pointc.y);
    printf("Validity signature    : [ %d ]", valid);
    }

    return valid;
}

/* rcv_data = STS_1_data = pointd.x||pointd.y||nonce||Encryption[signature(pointd.x||pointd.y||pointc.x||pointc.y)]||MAC */
uint8_t verify_STS_1(uint8_t *rcv_data, Memory* mem){

	WORD i                          = 0;
	WORD start                      = 0;
    uint16_t valid                  = 1;
    WORD signature[SIZE]            = {0};
    WORD toBeSigned[SIZE]           = {0};
    uint8_t signature8[SIG_LEN]     = {0};
    uint8_t rcv_pointx[POINT_LEN]  = {0};
    uint8_t rcv_pointy[POINT_LEN]  = {0};

    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t ciphertext[SIG_LEN]={0};
    p256_affine     session_key_affine  = {0};

    initArray(mem->pointd.x, SIZE); 
    initArray(mem->pointd.y, SIZE); 
    initArray(mem->session_key, SIZE); 
    initArray8(mem->session_key8, CHACHA_KEY_LENGTH); 

    // 1. retrieve data (points + signature) and store them in memory
    start = 0;
    for(i = 0; i<POINT_LEN; i++){
        rcv_pointx[i] = rcv_data[i];
        rcv_pointy[i] = rcv_data[i+POINT_LEN];
    }
    start += 2*POINT_LEN;
    for(i = 0; i<CHACHA_NONCE_LENGTH; i++){
        nonce[i] = rcv_data[start + i];
    }
    start += CHACHA_NONCE_LENGTH;
    for(i = 0; i<SIG_LEN; i++){
        ciphertext[i] = rcv_data[start+i];
    }
    start += SIG_LEN;
    for(i = 0; i<MAC_TAG_LENGTH; i++){
        mac_tag[i] = rcv_data[start +i];
    }
    start += MAC_TAG_LENGTH;

    rawbyte2word(mem->pointd.x, rcv_pointx, POINT_LEN); // store in memory
    rawbyte2word(mem->pointd.y, rcv_pointy, POINT_LEN);


    // 2. compute key
    pointScalarMultAffineWord(&session_key_affine, &mem->pointd, mem->control_secret);     // compute session point
    hash(mem->session_key,session_key_affine.x,SIZEHASH);    //compute session key by taking hash of session key x coordinate

    // 3. verify mac
    word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH);
    verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //this is not working

    // 4. decrypt
    encrypt_init(signature8,mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, 0);

    // 5. verify signature
    rawbyte2word(signature, signature8, SIG_LEN);
    for(i=1;i<=POINT_LEN/BYTE;i++){
        toBeSigned[i]                   = mem->pointd.x[i];
        toBeSigned[i+POINT_LEN/BYTE]    = mem->pointd.y[i];
        toBeSigned[i+2*POINT_LEN/BYTE]  = mem->pointc.x[i];
        toBeSigned[i+3*POINT_LEN/BYTE]  = mem->pointc.y[i];
    }
    toBeSigned[0] = 4*POINT_LEN/BYTE;
    valid = sig_ver(signature, mem->N,toBeSigned, mem->G.x,  mem->G.y, mem->drone_PK.x , mem->drone_PK.y );

    // 6. prints 
    if(mem->print){
    printf("[verify_STS_1_data] session_key x affine----- ");print_hex_type(session_key_affine.x,16);
    printf("[verify_STS_1_data] session_key y affine----- ");print_hex_type(session_key_affine.y,16);
    printf("[verify_STS_1_data] rcv drone's point x : ");    print_num(mem->pointd.x);
    printf("[verify_STS_1_data] rcv drone's point y : ");    print_num(mem->pointd.y);
    printf("[verify_STS_1_data] rcv nonce           : ");        print_array8(nonce, CHACHA_NONCE_LENGTH);
    printf("[verify_STS_1_data] computed key        : ");       print_num(mem->session_key);
    printf("[verify_STS_1_data] computed key 8      : ");       print_array8(mem->session_key8, CHACHA_KEY_LENGTH);
    printf("[verify_STS_1_data] rcv cipher          : ");       print_array8(ciphertext, SIG_LEN);
    printf("[verify_STS_1_data] rcv mac             : ");          print_array8(mac_tag, MAC_TAG_LENGTH);
    printf("[verify_STS_1_data] decrypt cipher (=sig): "); print_array8(signature8, SIG_LEN);
    printf("[verify_STS_1_data] rcv sig8            : ");          print_array8(signature8, SIG_LEN);
    printf("[verify_STS_1] toBeSigned     : ");         print_hex_type(toBeSigned, BIT);
    printf("[verify_STS_1] public KEY x     : ");       print_hex_type(mem->drone_PK.x , BIT);
    printf("[verify_STS_1] public KEY y     : ");       print_hex_type(mem->drone_PK.y , BIT);
    printf("[verify_STS_1] SIGNATURE      : ");         print_hex_type(signature, BIT);    
    printf("[verify_STS_1_data] signature valid? [ %d ]  \n\n ", valid);
    }

    return valid;
}


/* rcv_data = STS_2_data = Encryption[signature(pointc.x||pointc.||pointd.x||pointd.y]||MAC */
uint8_t verify_STS_2(uint8_t *rcv_data, Memory* mem){    //rcv_data = STS_2_data = Encryption[signature(pointc.x||pointd.x)]

    uint8_t ciphertext[SIG_LEN]={0};
    uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
    uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
    uint8_t signature8[SIG_LEN] = {0};
    WORD signature[SIZE] = {0};
    WORD toBeSigned[SIZE] = {0};

	WORD i = 0;
    uint8_t valid = 1;
    uint16_t start = 0;
    uint16_t len = 0;
 
    // 1. retrieve encrypted signature
    start = 0;
    for(i = 0; i<CHACHA_NONCE_LENGTH; i++){
        nonce[i] = rcv_data[start + i];
    }
    start += CHACHA_NONCE_LENGTH;
    for(i = 0; i<SIG_LEN; i++){
        ciphertext[i] = rcv_data[start + i];
    }
    start += SIG_LEN;
    for(i = 0; i<MAC_TAG_LENGTH; i++){
        mac_tag[i] = rcv_data[start + i];
    }
    start += MAC_TAG_LENGTH;
    len = start;
    assert(len = CHACHA_NONCE_LENGTH + SIG_LEN + MAC_TAG_LENGTH); // check


    // 2. verify mac
    verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //this is not working

    // 3. decrypt
    encrypt_init(signature8,mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, 0);
    rawbyte2word(signature, signature8, SIG_LEN);

    // 4. verify signature
    for(i=1;i<=POINT_LEN/BYTE;i++){
        toBeSigned[i]                   = mem->pointc.x[i];
        toBeSigned[i+POINT_LEN/BYTE]    = mem->pointc.y[i];
        toBeSigned[i+2*POINT_LEN/BYTE]  = mem->pointd.x[i];
        toBeSigned[i+3*POINT_LEN/BYTE]  = mem->pointd.y[i];
    }
    toBeSigned[0] = 4*POINT_LEN/BYTE;
    valid = sig_ver(signature, mem->N,toBeSigned, mem->G.x,  mem->G.y, mem->control_PK.x , mem->control_PK.y );

    // 5. prints    
    if(mem->print){
    printf("drone's point x : ");    print_num(mem->pointd.x);
    printf("drone's point y : ");    print_num(mem->pointd.y);
    printf("cc's    point x : ");    print_num(mem->pointc.x);
    printf("cc's    point y : ");    print_num(mem->pointc.y);
    printf("\n");
    printf("[make_STS_2] session key: ");    print_num(mem->session_key);
    printf("[verify_STS_2] received nonce       : ");    print_array8(nonce, CHACHA_NONCE_LENGTH);
    printf("[verify_STS_2] received sig (cipher): ");    print_array8(ciphertext, SIG_LEN);
    printf("[verify_STS_2] received mac         : ");    print_array8(mac_tag, MAC_TAG_LENGTH);
    printf("[verify_STS_2] decrypted signature  : ");  print_num(signature);
    printf("[verify_STS_2] toBeSigned     : ");         print_hex_type(toBeSigned, BIT);
    printf("[verify_STS_2] public KEY x     : ");       print_hex_type(mem->control_PK.x , BIT);
    printf("[verify_STS_2] public KEY y     : ");       print_hex_type(mem->control_PK.y , BIT);
    printf("[verify_STS_2] SIGNATURE      : ");         print_hex_type(signature, BIT);    
    printf("[verify_STS_2_data] signature valid? [ %d ]  \n\n ", valid);}
    return valid;
}
