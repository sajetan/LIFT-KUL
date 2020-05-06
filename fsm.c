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
    convert(mem->control_SK, "f257a192dde44227b3568008ff73bcf599a5c45b32ab523b5b21ca582fef5a0a");
    convert(mem->drone_SK, "bf92c9eb8ff61db0cd3e378b22b3887613afcd1861b11dfbf211d19489f6a08b");
    //random(mem->drone_SK, SIZE_EC_KEY, &mem->pool);
    //random(mem->control_SK, SIZE_EC_KEY, &mem->pool);
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
    mem->send_buf_len = 0;
    initArray8(mem->rcv_STS_0,MAX_DATA_LENGTH);
    mem->rcv_STS_0_len = 0;
    mem->retransmissionCounter = 0;

    // debugging
    mem->counter = 0;
    mem->print = 1;

} // end initMemory



/*  Depending on which is the current state, the according packet will be constructed here.
    For the STS-related states, STS message 0, 1 and 2 are constructed here.
    The resulting packet will be stored in the memory, as well as its length. */
void make_packet(Memory* mem, WORD_TAG tag){
    
    uint8_t data[MAX_DATA_LENGTH] = {0};
	WORD_LEN len = 0;
    
    // depending on which is the current state, the according packet is constructed
    switch (tag){
        case TAG_STS_0:
            make_STS_0_data(data, mem, &len);    //STS_0_data = pointc.x||pointc.y
            tag = TAG_STS_0;
            break;
        case TAG_STS_1:
            make_STS_1_data(data, mem, &len);    //STS_1_data = pointd.x||pointc.x||Encryption[signature(pointd.x||pointc.x)]
            tag = TAG_STS_1;
            break;
        case TAG_STS_2:
            make_STS_2_data(data, mem, &len);    //STS_2_data = Encryption[signature(pointc.x||pointd.x)]
            tag = TAG_STS_2;
            break;
        case TAG_STS_OK:
            len = 0;
            tag = TAG_STS_OK;
            break;
        default:
            printf("problem here: a make function is not defined for this state (%s)\n", __func__);
            printf("tag: %d\n", tag);
            assert(0);
            break;
    }
    // assemble the packet (TLV format)
    getTLV(mem->send_buf, &mem->send_buf_len, tag, len, mem->receiverID, data);
}

/*  This function sends the packet over the udp socket */
void send_packet(uint8_t* buf, uint16_t buf_len){
    assert(buf_len<=MAX_TRANSFER_LENGTH);
    
    if(send_message(buf, buf_len)==0) {
		close_sockets();
		printf("Send failed in %s\n", __func__);
		return 0;
	}
    // print sent message
    if(PRINT_CONTENT_UDP_PACKET){
        printf("Sent message:    \n");
        for(int i = 0; i<buf_len; i++){
            printf("%02x ", buf[i]);
        }
        printf("\n");
    }
}

/*  Here we receive messages and verify their correctness.
    Returns 0 if incorrect message, 1 otherwise.
    Following checks are performed: id, tag and signature(only for STS messages)  */
uint16_t receive_packet(WORD_TAG *tag, WORD_LEN *len, uint8_t* data, WORD_ID id, uint16_t* timeout){
    uint16_t rcv_buf_len = 0;
    WORD_ID rcv_id = 0;
    uint8_t rcv_buf[MAX_TRANSFER_LENGTH] = {0};
    uint16_t valid = 0;
    uint16_t dropPacket = 0;

    initArray8(data, MAX_DATA_LENGTH);
    
    //receive message
    rcv_buf_len = receive_message(rcv_buf);

    
    if(rcv_buf_len != (uint16_t)~0){
        // simulate packet loss
        dropPacket = (rand()%99) < SIMULATE_PACKET_DROP;      // Returns a pseudo-random integer between 0 and RAND_MAX.
        if(dropPacket){
            rcv_buf_len = (uint16_t)~0;
            DEBUG_FSM("* packet received, but dropped *")
        }
        else{
            DEBUG_FSM("packet received, passed")
        }
    }

    // process received message
    if(rcv_buf_len == (uint16_t)~0){
        //DEBUG_FSM("timeout socket")
        *timeout = 1;
        valid  = 0;
    } else {
        *timeout = 0;
        decomposeTLV( tag,  len, &rcv_id, data, rcv_buf, rcv_buf_len);

        // print received message
        if(PRINT_CONTENT_UDP_PACKET){
            printf("Received: \n");
            printf("\t tag = %d\n", *tag);
            printf("\t len = %d\n", *len);
            printf("\t id  = %d\n", rcv_id);
            printf("\t data = ");
            print_array8(data, *len);
            printf("\n");
        }

        // perform the checks
        if(rcv_id != id){
            valid = 0;
        } else{
            // add integrity check here
            valid = 1;
        }
    }
    return valid;
}


/* idle state of the control center, its FSM always starts here */
State idle_CC_fct( Memory* mem){
    PRINT_STATE()
    return STS_make_0;
    //return key_exchange_CC;
}

/* idle state of the drone, its FSM always starts here */
State idle_drone_fct( Memory* mem){
    PRINT_STATE()
    return STS_receive_0;
    //return key_exchange_drone;
}

State STS_make_0_fct( Memory* mem){
    PRINT_STATE()
    mem->retransmissionCounter = 0;
    mem->counter++;
    printf("\n\n test Nr %d \n\n\n", mem->counter);
    make_packet(mem, TAG_STS_0);
    return STS_send_0;
}

State STS_make_1_fct( Memory* mem){
    PRINT_STATE()
    mem->counter++;
    printf("\n\n test Nr %d \n\n\n", mem->counter);
    make_packet(mem, TAG_STS_1);
    return STS_send_1;
}

State STS_make_2_fct(Memory* mem){
    PRINT_STATE()
    mem->retransmissionCounter = 0;
    make_packet(mem, TAG_STS_2);
    return STS_send_2;
}


State STS_send_0_fct(Memory* mem){
    PRINT_STATE()
    send_packet(mem->send_buf, mem->send_buf_len);
    return STS_receive_1;
}

State STS_send_1_fct(Memory* mem){
    PRINT_STATE()
    send_packet(mem->send_buf, mem->send_buf_len);
    return STS_receive_2;
}

State STS_send_2_fct(Memory* mem){
    PRINT_STATE()
    send_packet(mem->send_buf, mem->send_buf_len);
    return STS_receive_OK;
}

/*  Here we wait for STS0 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_0_fct(Memory* mem){
    PRINT_STATE()
    uint8_t data[MAX_DATA_LENGTH] = {0};
    uint16_t timeoutSocket = 0;
    uint16_t restart = 0;
    uint16_t valid = 0;
    
    WORD_LEN len = 0;
    WORD_TAG tag = TAG_UNDEFINED;
    Timer myTimer;
    
    startTimer(&myTimer);

    while(1){
        initArray8(data, MAX_DATA_LENGTH);
        valid = receive_packet( &tag, &len, data, mem->receiverID, &timeoutSocket);
        if(timeoutSocket){
            // if timeout, just continue listening
        } else if(!valid){
            DEBUG_FSM("invalid id or incorrect integrity check")
        } else{
            DEBUG_FSM("valid id and correct integrity check")
            if(tag == TAG_STS_0){
                restart = 1;
                if(len == mem->rcv_STS_0_len){  // compare STS_0 message with the one in memory
                    if(equalArray8(data, mem->rcv_STS_0, len)){
                        restart = 0;
                    } 
                } 
                if(restart){
                    DEBUG_FSM("STS_0 received again, but different: restart protocol")
                    printf("------- STS 0 OK ----------\n");
                    initArray8(mem->rcv_STS_0, MAX_DATA_LENGTH);
                    copyArray8(mem->rcv_STS_0, data, len );
                    mem->rcv_STS_0_len = len;
                    valid = verify_STS_0(data, mem); // valid is always 1
                    return STS_make_1;
                } else{
                    DEBUG_FSM("STS_0 received again and ignored")
                }
            }
            else{
                DEBUG_FSM("unexpected tag, message ignored")
            }
        }
    }   
}


/*  Here we wait for STS1 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_1_fct(Memory* mem){

    PRINT_STATE()
    uint16_t valid = 0;
    WORD_TAG tag = TAG_UNDEFINED;
    WORD_LEN len = 0;
    uint8_t data[MAX_DATA_LENGTH] = {0};
    uint16_t timeoutSocket = 0;
    Timer myTimer;
    startTimer(&myTimer);

    while(valueTimer(&myTimer)<TIMEOUT){
        initArray8(data, MAX_DATA_LENGTH);
        valid = receive_packet( &tag, &len, data, mem->receiverID, &timeoutSocket);

        if(timeoutSocket){
            // timeout of UDP socket
        } else if(!valid){
            DEBUG_FSM("invalid id or incorrect integrity check")
        } else{
            DEBUG_FSM("valid id and correct integrity check")
            if(tag == TAG_STS_1){
                DEBUG_FSM("correct tag")
                valid = verify_STS_1(data, mem);
                if(valid){
                    DEBUG_FSM("valid signature, send next message")
                    printf("------- STS 1 OK ----------\n");
                    return STS_make_2;
                } else{
                    DEBUG_FSM("invalid signature, continue listening")
                }
            } else{
                DEBUG_FSM("unexpected tag, message ignored")
            }
        }
    }
    mem->retransmissionCounter++;
    if(mem->retransmissionCounter < MAX_TRIALS){
        DEBUG_FSM("timeout, send previous message again")
        return STS_send_0;
    } else{
        DEBUG_FSM("timeout and retransmission counter exceeded, restart protocol")
        return STS_make_0;
    }
}

/*  Here we wait for STS2 to arrive.
    Based on the validity of the message, the next state is defined */
State STS_receive_2_fct(Memory* mem){
    PRINT_STATE()
    uint16_t valid = 0;
    WORD_TAG tag = TAG_UNDEFINED;
    WORD_LEN len = 0;
    uint8_t data[MAX_DATA_LENGTH] = {0};
    uint16_t timeoutSocket = 0;
    uint16_t restart = 0;
    Timer myTimer;
    startTimer(&myTimer);

    while(valueTimer(&myTimer)<TIMEOUT){
        initArray8(data, MAX_DATA_LENGTH);
        valid = receive_packet( &tag, &len, data, mem->receiverID, &timeoutSocket);

        if(timeoutSocket){
            // timeout of UDP socket
        } else if(!valid){
            DEBUG_FSM("invalid id or incorrect integrity check")
        } else{
            DEBUG_FSM("valid id and correct integrity check")
            if(tag == TAG_STS_2){
                DEBUG_FSM("correct tag")
                valid = verify_STS_2(data, mem);
                if(valid){
                    DEBUG_FSM("correct signature")
                    printf("------- STS 2 OK ----------\n");
                    if(INFINITE_LOOP_STS){
                        return STS_send_OK;
                    } else{
                        return State_Exit;
                    }
                } else{
                    DEBUG_FSM("invalid signature, continue listening")
                }
            } else if(tag == TAG_STS_0){
                restart = 1;
                if(len == mem->rcv_STS_0_len){  // compare STS_0 message with the one in memory
                    if(equalArray8(data, mem->rcv_STS_0, len)){
                        restart = 0;
                    } 
                } 
                if(restart){
                    DEBUG_FSM("STS_0 received again, but different: restart protocol")
                    initArray8(mem->rcv_STS_0, MAX_DATA_LENGTH);
                    copyArray8(mem->rcv_STS_0, data, len );
                    mem->rcv_STS_0_len = len;
                    valid = verify_STS_0(data, mem); // valid is always 1
                    return STS_make_1;
                } else{
                    DEBUG_FSM("STS_0 received again and ignored")
                }
            } else{
                DEBUG_FSM("unexpected tag, message ignored")
            }
        }
    }
    DEBUG_FSM("timeout, send previous message again")
    return STS_send_1;
}

State STS_receive_OK_fct(Memory* mem){
    PRINT_STATE()
    uint16_t valid = 0;
    WORD_TAG tag = TAG_UNDEFINED;
    WORD_LEN len = 0;
    uint8_t data[MAX_DATA_LENGTH] = {0};
    uint16_t timeoutSocket = 0;
    Timer myTimer;
    startTimer(&myTimer);

    while(valueTimer(&myTimer)<TIMEOUT){
        initArray8(data, MAX_DATA_LENGTH);
        valid = receive_packet( &tag, &len, data, mem->receiverID, &timeoutSocket);

        if(timeoutSocket){
            // timeout of UDP socket
        } else if(!valid){
            DEBUG_FSM("invalid id or incorrect integrity check")
        } else{
            DEBUG_FSM("valid id and correct integrity check")
            if(tag == TAG_STS_OK){
                DEBUG_FSM("correct tag")
                if(INFINITE_LOOP_STS){
                    return STS_make_0;
                } else{
                    return State_Exit;
                }
            } else{
                DEBUG_FSM("unexpected tag, message ignored")
            }
        }
    }
    mem->retransmissionCounter++;
    if(mem->retransmissionCounter < MAX_TRIALS){
        DEBUG_FSM("timeout, send previous message again")
        return STS_send_2;
    } else{
        DEBUG_FSM("timout and retransmission counter exceeded, restart protocol")
        return STS_make_0;
    }
}

State STS_send_OK_fct(Memory* mem){
    PRINT_STATE()
    uint8_t rcv_buffer[MAX_TRANSFER_LENGTH] = {0};
    uint16_t rcv_buf_len = 0;
    WORD_TAG tag = TAG_UNDEFINED;
    WORD_LEN len = 0;
    uint8_t data[MAX_DATA_LENGTH] = {0};
    uint16_t timeoutSocket = 0;
    uint16_t valid = 0;
    Timer myTimer;
    
    while(1){
        // send OK packet
        make_packet(mem, TAG_STS_OK);
        send_packet(mem->send_buf, mem->send_buf_len);
        startTimer(&myTimer);

        while(valueTimer(&myTimer)<TIMEOUT){
            initArray8(data, MAX_DATA_LENGTH);
            valid = receive_packet( &tag, &len, data, mem->receiverID, &timeoutSocket);

            if(timeoutSocket){
                // timeout of UDP socket
            } else if(!valid){
                DEBUG_FSM("invalid id or incorrect integrity check")
            } else{
                DEBUG_FSM("valid id and correct integrity check")
                if(tag == TAG_STS_0){
                    printf("------- STS 0 OK ----------\n");
                    DEBUG_FSM("start protocol again")
                    valid = verify_STS_0(data, mem); // valid is always 1
                    initArray8(mem->rcv_STS_0, MAX_DATA_LENGTH);
                    copyArray8(mem->rcv_STS_0, data, len); // copy STS_0 in memory for later comparison
                    mem->rcv_STS_0_len = len;
                    return STS_make_1;
                } else{
                    DEBUG_FSM("unexpected tag, message ignored")
                }
            }
        }
        DEBUG_FSM("timeout, send previous OK message again")
    }
}
State key_exchange_CC_fct(Memory* memory){

    uint8_t buffer[MAX_TRANSFER_LENGTH] = {0};
    uint16_t buf_len = 0;
    uint8_t rcv_buffer[MAX_TRANSFER_LENGTH] = {0};
    uint16_t rcv_buf_len = 0;
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

    do{
        // send message
        if(send_message(buffer, buf_len)==0) {
            close_sockets();
            printf("Send failed at message STS2 \n");
            return 0;
        }

        rcv_buf_len = receive_message(rcv_buffer);
        for(uint16_t i = 0; i<POINT_LEN; i++){
            pointx[i] = rcv_buffer[i];
            pointy[i] = rcv_buffer[i+POINT_LEN];
        }
    } while(rcv_buf_len == (uint16_t) ~0);

    rawbyte2word(pk_other->x , pointx, POINT_LEN);
    rawbyte2word(pk_other->y , pointy, POINT_LEN);

    if(DEBUG_SIGNATURE){
        printf("SK control:     : ");       print_hex_type(memory->drone_SK , BIT);
        printf("PK control:     : ");       print_hex_type(memory->control_PK.x , BIT);
        printf("PK control:     : ");       print_hex_type(memory->control_PK.y, BIT);
        printf("PK drone:     : ");       print_hex_type(memory->drone_PK.x , BIT);
        printf("PK drone:     : ");       print_hex_type(memory->drone_PK.y , BIT);
    }


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

    do{
        buf_len = receive_message(buffer);
    } while(buf_len == (uint16_t) ~0);

    
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

    if(DEBUG_SIGNATURE){
        printf("SK drone:     : ");       print_hex_type(memory->drone_SK , BIT);
        printf("PK drone:     : ");       print_hex_type(memory->drone_PK.x , BIT);
        printf("PK drone:     : ");       print_hex_type(memory->drone_PK.y , BIT);
        printf("PK control:     : ");       print_hex_type(memory->control_PK.x , BIT);
        printf("PK control:     : ");       print_hex_type(memory->control_PK.y, BIT);
    }

    return STS_receive_0;
}












/* STS_0_data = pointc.x||pointc.y  (both coordinates are 256 bits => total of 526 bits of data )*/
void make_STS_0_data(uint8_t *data, Memory* mem, WORD_LEN *len){     

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
    if(DEBUG_SIGNATURE){
        printf("control center's private key        :"); print_hex_type(privateKey,16);
        printf("control center's public key.x-----  :");print_hex_type(publicKey->x,16);
        printf("control center's public key.y-----  :");print_hex_type(publicKey->y,16);
        printf("data x                      -----  :");print_array(publicKey->x,SIZE);
        printf("data y                      -----  :");print_array(publicKey->y,SIZE);
        printf("data x                      -----  :");print_array8(pointx8,POINT_LEN);
        printf("data y                      -----  :");print_array8(pointy8,POINT_LEN);
        printf("data                       -----  :");print_array8(data,2*POINT_LEN);   
    }

}
/* STS_1_data = pointd.x||pointc.x||Encryption[signature(pointd.x||pointd.x||pointc.x||pointc.x)] */
void make_STS_1_data(uint8_t *data, Memory* mem, WORD_LEN *len){  

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
    if(DEBUG_SIGNATURE){
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
    }
}

/* STS_2_data = Encryption[signature(pointc.x||pointc.y||pointd.x||pointd.y)] */
void make_STS_2_data(uint8_t *data, Memory* mem, WORD_LEN *len){   

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
    if(DEBUG_SIGNATURE){
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
uint16_t verify_STS_0(uint8_t *rcv_data, Memory* mem){  

    WORD i = 0;
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
    if(DEBUG_SIGNATURE){  
        printf("received cc's point x : ");    print_num(mem->pointc.x);
        printf("received cc's point y : ");    print_num(mem->pointc.y);
    }

    return 1;
}

/* rcv_data = STS_1_data = pointd.x||pointd.y||nonce||Encryption[signature(pointd.x||pointd.y||pointc.x||pointc.y)]||MAC */
uint16_t verify_STS_1(uint8_t *rcv_data, Memory* mem){

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
    valid = verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //this is not working
    
    if(DEBUG_SIGNATURE){
        printf("[verify_STS_1_data] session_key x affine----- ");print_hex_type(session_key_affine.x,16);
        printf("[verify_STS_1_data] session_key y affine----- ");print_hex_type(session_key_affine.y,16);
        printf("[verify_STS_1_data] rcv drone's point x : ");    print_num(mem->pointd.x);
        printf("[verify_STS_1_data] rcv drone's point y : ");    print_num(mem->pointd.y);
        printf("[verify_STS_1_data] rcv nonce           : ");        print_array8(nonce, CHACHA_NONCE_LENGTH);
        printf("[verify_STS_1_data] computed key        : ");       print_num(mem->session_key);
        printf("[verify_STS_1_data] computed key 8      : ");       print_array8(mem->session_key8, CHACHA_KEY_LENGTH);
        printf("[verify_STS_1_data] rcv cipher          : ");       print_array8(ciphertext, SIG_LEN);
        printf("[verify_STS_1_data] rcv mac             : ");          print_array8(mac_tag, MAC_TAG_LENGTH);
    }
    if(!valid){
        return 0;
    }

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
    if(DEBUG_SIGNATURE){
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
uint16_t verify_STS_2(uint8_t *rcv_data, Memory* mem){    //rcv_data = STS_2_data = Encryption[signature(pointc.x||pointd.x)]

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
    valid = verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, "50515253c0c1c2c3c4c5c6c7"); //this is not working
    if(DEBUG_SIGNATURE){
        printf("drone's point x : ");    print_num(mem->pointd.x);
        printf("drone's point y : ");    print_num(mem->pointd.y);
        printf("cc's    point x : ");    print_num(mem->pointc.x);
        printf("cc's    point y : ");    print_num(mem->pointc.y);
        printf("\n");
        printf("[make_STS_2] session key: ");    print_num(mem->session_key);
        printf("[verify_STS_2] received nonce       : ");    print_array8(nonce, CHACHA_NONCE_LENGTH);
        printf("[verify_STS_2] received sig (cipher): ");    print_array8(ciphertext, SIG_LEN);
        printf("[verify_STS_2] received mac         : ");    print_array8(mac_tag, MAC_TAG_LENGTH);
    }
    
    if(!valid){
        return 0;
    }

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
    if(DEBUG_SIGNATURE){
        printf("[verify_STS_2] decrypted signature  : ");  print_num(signature);
        printf("[verify_STS_2] toBeSigned     : ");         print_hex_type(toBeSigned, BIT);
        printf("[verify_STS_2] public KEY x     : ");       print_hex_type(mem->control_PK.x , BIT);
        printf("[verify_STS_2] public KEY y     : ");       print_hex_type(mem->control_PK.y , BIT);
        printf("[verify_STS_2] SIGNATURE      : ");         print_hex_type(signature, BIT);    
        printf("[verify_STS_2_data] signature valid? [ %d ]  \n\n ", valid);
    }
    return valid;
}
