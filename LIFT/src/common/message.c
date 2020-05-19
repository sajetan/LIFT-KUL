/*
 * message.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "message.h"
#include "crc.h"

void getTLV(uint8_t* buf, uint16_t* buf_len, WORD_TAG tag,  WORD_LEN dataLength, WORD_ID id, uint8_t* data){
	assert(MAX_DATA_LENGTH >= dataLength);

	uint16_t i = 0;
	uint16_t start = 0;
	WORD_LEN dataLengthCopy = dataLength;

	initArray8(buf, MAX_TRANSFER_LENGTH);

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
	uint32_t crc= compute_crc(buf, start);
	//printf("Sending [crc = %08x ]\n",crc);
	memcpy(buf+start, &crc, sizeof(crc));
	start+=sizeof(crc);

	assert(start == (BYTE_TAG + BYTE_LEN + BYTE_ID + dataLength+sizeof(crc)));

	// return length
	*buf_len = start;
	assert(MAX_TRANSFER_LENGTH >= *buf_len);

}

LIFT_RESULT decomposeTLV( WORD_TAG* tag,  WORD_LEN* dataLength, uint32_t *crc, WORD_ID* id, uint8_t* data, uint8_t* buf, uint16_t buf_len){
	assert(MAX_TRANSFER_LENGTH >= buf_len);

	struct lift_pkt_hdr *hdr = (struct lift_pkt_hdr *) buf;
	uint16_t i = 0;
	uint16_t start = 0;
	uint32_t rcvd_crc= *((uint32_t *)(buf + sizeof(*hdr) + hdr->len));

	uint32_t calcrc = compute_crc(buf,sizeof(*hdr)+hdr->len);

	if(DEBUG){
		printf("tag = %d \n",hdr->tag);
	    printf("len = %d \n",hdr->len);
	    printf("id = %d \n",hdr->id);
		printf("this is crc = %8x \n",rcvd_crc);
		printf("calccrc = %8x \n",calcrc);

	}

	if (rcvd_crc!=calcrc) {printf("CRC Incorrect, Packet will be dropped \n");return RETURN_INVALID;}

	//initialize
	*tag = hdr->tag;
	*dataLength = hdr->len;
	*id = hdr->id;
	*crc=calcrc;
	initArray8(data, MAX_DATA_LENGTH);

	// insert data
	start += sizeof(*hdr);
	for(i = 0; i<*dataLength; i++){
		data[i] = buf[start + i];
	}

	// perform check
	start += i;
	assert(start == (BYTE_TAG + BYTE_LEN + BYTE_ID + *dataLength));
	return RETURN_SUCCESS; //add return macros
}

