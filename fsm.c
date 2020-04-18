/*
 * fsm.c
 * 
 *  Created on: April 18, 2020
 *      Author: Ferdinand Hannequart
 */

#include"fsm.h"

void initMemory(Memory* mem){

    mem->receiverID = 117;
  
}



State STS_send_0_fct(uint8_t* buf, uint16_t* buf_len, Memory* mem){
    printf("\n/////\n State STS_send_0 \n");

    // compute new message
	uint8_t data[10] ="aaaa";
	WORD_LEN len = 4;
    getTLV(buf, buf_len, TAG_STS_0, len, mem->receiverID, data);

    // send message
    if(send_message(buf, *buf_len)==0) {
		close_sockets();
		printf("Send failed at message tag %d\n", TAG_STS_0);
		return 0;
	}

    // print sent message
	printf("Sent message STS 0:    \n");
	for(int i = 0; i<*buf_len; i++){
		printf("%x ", buf[i]);
	}
    return STS_send_2;
}

State STS_send_2_fct(uint8_t* buf, uint16_t* buf_len, Memory* mem){
    printf("\n//////\nState STS_send_2 \n");

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
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, *buf_len);

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

    if(rcv_tag != TAG_STS_1){
        printf("Invalid tag due to timeout or error, return in first state\n");
        return STS_send_0;
    }
    else{
        printf("Valid tag, send next message\n");

        // compute new message
        getTLV(buf, buf_len, TAG_STS_2, rcv_data_len, mem->receiverID, rcv_data);

        // send message
        if(send_message(buf, *buf_len)==0) {
            close_sockets();
            printf("Send failed at message STS2 \n");
            return 0;
        }
        
        // print sent message
        printf("Sent message STS 2:    ");
        for(int i = 0; i<*buf_len; i++){
            printf("%x ", buf[i]);
        }

        return STS_CC_completed;
    }
}

State STS_send_1_fct(uint8_t* buf, uint16_t* buf_len, Memory* mem){
    printf("\n//////\nState STS_send_1 \n");

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
    decomposeTLV( &rcv_tag,  &rcv_data_len, &rcv_id, rcv_data, buf, *buf_len);

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
        getTLV(buf, buf_len, TAG_STS_1, rcv_data_len, mem->receiverID, rcv_data);

        // send message
        if(send_message(buf, *buf_len)==0) {
            close_sockets();
            printf("Send failed at message STS1 \n");
            return 0;
        }
        
        // print sent message
        printf("Sent message STS 1:    ");
        for(int i = 0; i<*buf_len; i++){
            printf("%x ", buf[i]);
        }

        return STS_drone_completed;
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

