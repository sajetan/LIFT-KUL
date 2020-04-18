/*
 * message.c
 * 
 *  Created on: April 9, 2020
 *      Author: Ferdinand Hannequart
 */

#include"message.h"

void getTLV(uint8_t* buf, uint16_t* buf_len, WORD_TAG tag,  WORD_LEN dataLength, WORD_ID id, uint8_t* data){
    assert(MAX_DATA_LENGTH >= dataLength);

    uint16_t i = 0;
    uint16_t start = 0;
    WORD_LEN dataLengthCopy = dataLength;

    // insert tag
    start = 0;
    for(i = 0; i<BYTE_TAG; i++){
        tag >>= 8*i;
        buf[start + i] = tag;
    }
    
    // insert length
    start += i;
    for(i = 0; i<BYTE_LEN; i++){
        dataLengthCopy >>= 8*i;
        buf[start + i] = dataLengthCopy;
    }
    
    // insert ID
    start += i;
    for(i = 0; i<BYTE_ID; i++){
        id >>= 8*i;
        buf[start + i] = id;
    }

    // insert data
    start += i;
    for(i = 0; i<dataLength; i++){
        buf[start + i] = data[i];
    }

    // perform check
    start += i;
    assert(start == (BYTE_TAG + BYTE_LEN + BYTE_ID + dataLength));

    // return length
    *buf_len = start;
}

void decomposeTLV( WORD_TAG* tag,  WORD_LEN* dataLength, WORD_ID* id, uint8_t* data, uint8_t* buf, uint16_t buf_len){
    assert(MAX_TRANSFER_LENGTH >= buf_len);

    uint16_t i = 0;
    uint16_t start = 0;

    //initialize
    *tag = 0;
    *dataLength = 0;
    *id = 0;

    // copy tag
    start = 0;
    for(i = 0; i<BYTE_TAG; i++){
        buf[start + i] >>= 8*i;
        *tag |= buf[start + i];
    }
    
    // insert length
    start += i;
    for(i = 0; i<BYTE_LEN; i++){
        buf[start + i] >>= 8*i;
        *dataLength |= buf[start + i];
    }
    
    // insert ID
    start += i;
    for(i = 0; i<BYTE_ID; i++){
        buf[start + i] >>= 8*i;
        *id |= buf[start + i] ;
    }

    // insert data
    start += i;
    for(i = 0; i<*dataLength; i++){
        data[i] = buf[start + i];
    }

    // perform check
    start += i;
    assert(start == (BYTE_TAG + BYTE_LEN + BYTE_ID + *dataLength));
}
/*
void getTLVTest(){
    WORD_TAG tag    = 0;
    WORD_LEN len    = 0;
    WORD_ID id      = 0;
    uint8_t data[BYTE_DATA_MAX] = {0};

    uint8_t message[BYTE_MESSAGE_MAX] = {0};
    int message_len = 0;

    tag = TAG_STS_1;    // send 2nd STS message
    id = 77;            // my id is 77
    data[0] = 255;
    data[1] = 256;
    data[BYTE_DATA_MAX-2] = 255;
    len = BYTE_DATA_MAX - 1;

    getTLV( message, &message_len,tag, len, id, data);

    print_array(message, message_len);
}
*/