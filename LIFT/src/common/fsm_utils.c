/*
 * fsm_utils.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "fsm.h"

void initMemory(Memory* mem){
	printf("Initializing memories\n");
	// entropy pool
	initPool(&mem->pool);
	accumulate(&mem->pool, 100);

	// parameters
	initArray(mem->N,SIZE);
	convert(mem->N,p256_curve_parameter_n_arr);
	initArray(mem->G.x,SIZE);
	initArray(mem->G.y,SIZE);
	convert(mem->G.x, p256_curve_parameter_gx_arr);
	convert(mem->G.y, p256_curve_parameter_gy_arr);
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
	//random_gen(mem->drone_SK, SIZE_EC_KEY, &mem->pool);
	//random_gen(mem->control_SK, SIZE_EC_KEY, &mem->pool);
	pointScalarMultAffineWord(&mem->drone_PK, &mem->G, mem->drone_SK);
	pointScalarMultAffineWord(&mem->control_PK, &mem->G, mem->control_SK);

	// session specific
	initArray(mem->session_drone_secret,SIZE);
	initArray(mem->session_drone_public.x,SIZE);
	initArray(mem->session_drone_public.y,SIZE);
	initArray(mem->session_control_secret,SIZE);
	initArray(mem->session_control_public.x,SIZE);
	initArray(mem->session_control_public.y,SIZE);
	initArray(mem->session_key,SIZE);
	initArray8(mem->session_key8,CHACHA_KEY_LENGTH);

	// communication specific
	initArray8(mem->send_buf,MAX_TRANSFER_LENGTH);
	mem->send_buf_len = 0;
	initArray8(mem->rcv_STS_0,MAX_DATA_LENGTH);
	mem->rcv_STS_0_len = 0;
	initArray8(mem->send_vidbuf,MAX_TRANSFER_LENGTH);
	mem->send_vidbuf_len = 0;

	// debugging
	mem->counter = 0;
	mem->mean = 0;

	//new items
	mem->current_state=0;
	mem->cmd=0;
	mem->cmd_type=0;
	mem->seq_num=0;
	mem->is_videostreaming=0;
	mem->vid_seq_num=0;

} // end initMemory




void compose_command_response(uint8_t* response_payload, uint32_t seq_num,  WORD_TAG cmd_type,  WORD_LEN cmd, uint8_t* response, WORD_LEN response_length){
	uint16_t i = 0;
	uint16_t start = 0;
	initArray8(response_payload, MIN_PACKET_LENGTH);

	// insert sequence number
	start = 0;
	for(i = 0; i<SEQ_LENGTH; i++){
		response_payload[start + i] = (seq_num >> (8*i));
	}
	start += SEQ_LENGTH;

	// insert command type
	for(i = 0; i<COMMAND_LENGTH; i++){
		response_payload[start + i] = (cmd_type >> (8*i));
	}
	start += COMMAND_LENGTH;

	// insert command
	for(i = 0; i<COMMAND_LENGTH; i++){
		response_payload[start + i] = (cmd >> (8*i));
	}
	start += COMMAND_LENGTH;

	// insert response
	for(i = 0; i<response_length; i++){
		response_payload[start + i] = response[i];
	}
	start += response_length;


	//    printf("data compose: \n");
	//    for(i = 0; i<MIN_PACKET_LENGTH; i++){
	//        printf("%02x ",response_payload[i]);
	//    }

}


void decompose_command_response(uint8_t *data, uint32_t *seq_num,  WORD_TAG *cmd_type,  WORD_LEN *cmd, uint8_t* response, WORD_LEN response_len){
	uint16_t i = 0;
	initArray8(data, MAX_DATA_LENGTH);
	uint16_t data_len = response_len-sizeof(lift_cmd_pkt);

	struct lift_cmd_pkt *pkt = (struct lift_cmd_pkt *) response;
	*seq_num = pkt->seq_num;
	*cmd_type = pkt->cmd_type;
	*cmd = pkt->cmd;

	// insert response
	for(i = 0; i<data_len; i++){
		data[i] = response[sizeof(lift_cmd_pkt)+i];
	}

	//    printf("data decompose : \n");
	//    for(i = 0; i<MIN_PACKET_LENGTH; i++){
	//        printf("%02x ",data[i]);
	//    }

}


uint16_t make_encrypted_message(uint8_t* data, Memory* mem, uint8_t* plaintext, WORD_LEN plaintext_len){
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
	uint8_t ciphertext[MAX_MSG_SIZE]={0};
	uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
	WORD len=0;
	WORD i=0;
	WORD rand[SIZE] = {0};
	initArray8(data, MAX_DATA_LENGTH);

	random_gen(rand, CHACHA_NONCE_LENGTH*8, &mem->pool);
	word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);


	aead_chacha20_poly1305(mac_tag,ciphertext, mem->session_key8, CHACHA_KEY_LENGTH, nonce, plaintext, plaintext_len, "50515253c0c1c2c3c4c5c6c7"); //encrypt the signature with the session key k

	//sending plaintext length in the beginning
	data[0]=plaintext_len;
	data[1]=plaintext_len>>8;
	len += 2;

	for(i=0;i<CHACHA_NONCE_LENGTH;i++){
		data[len + i] = nonce[i];
	}
	len += CHACHA_NONCE_LENGTH;

	for(i=0;i<plaintext_len;i++){
		data[len + i] = ciphertext[i];
	}
	len +=plaintext_len;

	for(i=0;i<MAC_TAG_LENGTH;i++){
		data[len + i] = mac_tag[i];
	}
	len += MAC_TAG_LENGTH;


	assert((len = CHACHA_NONCE_LENGTH + plaintext_len + MAC_TAG_LENGTH+2)); // check
	return len;
}

LIFT_RESULT make_decrypted_message(uint8_t* plaintext, WORD_LEN* plaintext_len, Memory* mem, uint8_t* rcv_data){
	uint16_t ciphertext_len=0;
	uint8_t ciphertext[MAX_DATA_LENGTH]={0};
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
	uint8_t nonce[CHACHA_NONCE_LENGTH]={0};

	WORD i = 0;
	uint8_t valid = 0;
	uint16_t start = 0;


	// 1. retrieve encrypted message
	ciphertext_len = (rcv_data[1]<<8|rcv_data[0]);

	start += 2;
	for(i = 0; i<CHACHA_NONCE_LENGTH; i++){
		nonce[i] = rcv_data[start + i];
	}
	start += CHACHA_NONCE_LENGTH;

	for(i = 0; i<ciphertext_len; i++){
		ciphertext[i] = rcv_data[start + i];
	}
	start += ciphertext_len;

	for(i = 0; i<MAC_TAG_LENGTH; i++){
		mac_tag[i] = rcv_data[start + i];
	}
	start += MAC_TAG_LENGTH;

	// 1. verify mac
	valid = verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, ciphertext_len, "50515253c0c1c2c3c4c5c6c7");
	if(!valid){
		return RETURN_INVALID;
	}

	// 2. decrypt
	decrypt_init(plaintext,mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, ciphertext_len, 0);

	*plaintext_len=ciphertext_len;
	return RETURN_SUCCESS;
}


LIFT_RESULT make_command_request_packet(Memory* mem){
	uint8_t request_packet[MAX_DATA_LENGTH] = {0};
	uint8_t request_payload[MIN_PACKET_LENGTH]={0};
	WORD_LEN request_packet_len = 0;
	WORD_LEN len=0;
	//WORD i=0;

	if (mem->cmd_type==0)return RETURN_INVALID;
	mem->seq_num+=1; //increment sequence number

	printf("Send: sequence number: [%08x] cmd_type [%04x]  cmd [%04x] \n", mem->seq_num,mem->cmd_type,mem->cmd);

	// constructing packet
	request_payload[len] = mem->seq_num;
	request_payload[len+1]=mem->seq_num >>8;
	request_payload[len+2]=mem->seq_num >>16;
	request_payload[len+3]=mem->seq_num >>24;
	len+=SEQ_LENGTH;

	request_payload[len] = mem->cmd_type;
	request_payload[len+1]=mem->cmd_type >>8;
	len+=COMMAND_LENGTH;

	request_payload[len] = mem->cmd;
	request_payload[len+1]=mem->cmd >>8;
	len+=COMMAND_LENGTH;

	//	printf("plaintext data: \n");
	//	for (int p=0;p<MIN_PACKET_LENGTH;p++){
	//		printf("%02x ",request_payload[p]);
	//	}

	request_packet_len=make_encrypted_message(request_packet,mem, request_payload,MIN_PACKET_LENGTH);
	getTLV(mem->send_buf, &mem->send_buf_len, TAG_COMMAND, request_packet_len, mem->receiverID, request_packet);
	return RETURN_SUCCESS;
}

/*  This function sends the packet over the udp socket */
void send_packet(uint8_t* buf, uint16_t buf_len){
	assert((buf_len<=MAX_TRANSFER_LENGTH));
	//printf("inside sendloop len=%d \n", buf_len);
	//print_array8(buf, MAX_TRANSFER_LENGTH);
	if(SEND_CONSTANT_DATA)buf_len=MAX_TRANSFER_LENGTH;

	if(send_message(buf, buf_len)==0) {
		close_sockets();
		printf("Send failed in %s\n", __func__);
		return;
	}
	if(PRINT_CONTENT_UDP_PACKET){
		printf("Sent message    \n");
		for(int i = 0; i<buf_len; i++){
			printf("%02x ", buf[i]);
		}
		printf("\n");
	}
}

/*  Here we receive messages and verify their correctness.
    Returns 0 if incorrect message, 1 otherwise.
    Following checks are performed: id, tag and signature(only for STS messages)  */
LIFT_RESULT receive_packet(WORD_TAG *tag, WORD_LEN *len, uint32_t *crc, uint8_t* data, WORD_ID id, uint16_t* timeout){
	uint16_t rcv_buf_len = 0;
	WORD_ID rcv_id = 0;
	uint8_t rcv_buf[MAX_TRANSFER_LENGTH] = {0};
	uint16_t valid = 0;
	uint16_t dropPacket = 0;
	uint8_t mask = 0;
	uint8_t temp = 0;
	uint32_t r = 0;
	uint16_t i = 0;
	uint16_t bit_flip = 0;


	initArray8(data, MAX_DATA_LENGTH);

	//receive message
	rcv_buf_len = receive_message(rcv_buf);
	srand(time(0));
	// simulation part begin
	if(rcv_buf_len != (uint16_t)~0){
		// simulate packet loss
		dropPacket = (rand()%99) < SIMULATE_PACKET_DROP;
		if(dropPacket){
			rcv_buf_len = (uint16_t)~0;
		}else{
			// simulate bit flip
			if(IF_BITERROR){
				for(i = 0; i<rcv_buf_len; i++){
					r = (rand() % BER_INVERSE);
					if(r<3){
						bit_flip = 1;
						mask = 1<<r;
					} else{
						mask = 0;
					}
					temp = rcv_buf[i] & ~mask;

					if(temp == rcv_buf[i] ){
						temp = rcv_buf[i] | mask;
					}
					rcv_buf[i] = temp;

				}
			}
		}
	}
	if(dropPacket)  DEBUG_FSM("* [simulation] received packet is dropped *")
    		if(bit_flip)    DEBUG_FSM("* [simulation] received packet contains bit flips *")
			// simulation part end

			// process received message
			if(rcv_buf_len == (uint16_t)~0){
				//DEBUG_FSM("timeout socket")
				*timeout = 1;
				valid  = 0;
			} else {
				*timeout = 0;
				valid = decomposeTLV( tag,  len, crc, &rcv_id, data, rcv_buf, rcv_buf_len);

				// print received message
				if(PRINT_CONTENT_UDP_PACKET){
					printf("Received: \n");
					printf("\t tag = %d\n", *tag);
					printf("\t len = %d\n", *len);
					printf("\t id  = %d\n", rcv_id);
					printf("\t crc = %08x\n",*crc);
					printf("\t data = ");
					print_array8(data, *len);
					printf("\n");
				}

			}
	//printf("ret\n");
	return ((valid & (rcv_id==id)));
}

