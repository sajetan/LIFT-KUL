/*
 * fsm.c
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include <pthread.h>
#include "fsm.h"
#include <time.h>


pthread_t gvideostream_handler;

void *video_streaming_thread(void *argdata){
	Memory *memory = (Memory *) argdata;

	uint8_t videoframe[VIDEOFRAME_LENGTH] = {0};
	uint8_t data_payload[MAX_DATA_LENGTH] = {0};
	uint8_t data_packet[MAX_DATA_LENGTH] = {0};
	WORD_LEN data_packet_len=0;

	WORD rand[SIZE] = {0};
	int i=0;
	int n=0;
	uint32_t seq_num=0;

	printf("Starting Video Streaming thread\n");

	while(memory->current_state==STS_COMPLETED_DRONE && memory->is_videostreaming==1){
		memory->vid_seq_num+=1;
		seq_num = memory->vid_seq_num;
		for(i = 0; i<SEQ_LENGTH; i++){
			videoframe[i] = ((seq_num >> (8*i)));
		}

		i=SEQ_LENGTH;
		for (n=0;n<VIDEOFRAMES;n++){
			random_gen(rand, 256, &memory->pool);
			word2rawbyte(&videoframe[i], rand, (32)); //1
			i+=32;
		}

		compose_command_response(data_payload,0, SESSION_VIDEO_STREAM_REQUEST, DRONE_VIDEO_FRAME, videoframe, VIDEOFRAME_LENGTH);
		data_packet_len=make_encrypted_message(data_packet,memory, data_payload, VIDEOFRAME_LENGTH+sizeof(lift_cmd_pkt));

		// assemble the packet (TLV format)
		getTLV(memory->send_vidbuf, &memory->send_vidbuf_len, TAG_COMMAND, data_packet_len, memory->receiverID, data_packet);

		send_packet(memory->send_vidbuf, memory->send_vidbuf_len);
		//usleep(10); //streaming about 1072 bytes of data every 10microsecond
	}
	printf("Stopping video stream  total frames sent [%d] [total %.2f Mbytes] \n", memory->vid_seq_num, (double)(memory->vid_seq_num*VIDEOFRAMES*32/1000000.0));
	memory->vid_seq_num=0;
	pthread_exit(NULL);

}


/*  Depending on which is the current STATE, the according packet will be constructed here.
    For the STS-related STATEs, STS message 0, 1 and 2 are constructed here.
    The resulting packet will be stored in the memory, as well as its length. */
void make_packet(Memory* mem, WORD_TAG tag){

	uint8_t data[MAX_DATA_LENGTH] = {0};
	WORD_LEN len = 0;

	// depending on which is the current STATE, the according packet is constructed
	switch (tag){
	case TAG_STS_0:
		make_STS_0_data(data, mem, &len);    //STS_0_data = session_control_public.x||session_control_public.y
		tag = TAG_STS_0;
		break;
	case TAG_STS_1:
		make_STS_1_data(data, mem, &len);    //STS_1_data = session_drone_public.x||session_control_public.x||Encryption[signature(session_drone_public.x||session_control_public.x)]
		tag = TAG_STS_1;
		break;
	case TAG_STS_2:
		make_STS_2_data(data, mem, &len);    //STS_2_data = Encryption[signature(session_control_public.x||session_drone_public.x)]
		tag = TAG_STS_2;
		break;
	case TAG_STS_OK:    // OK message, this massage does not contain any data
		len = 0;
		tag = TAG_STS_OK;
		break;
	default:
		printf("problem here: a make function is not defined for this STATE (%s)\n", __func__);
		printf("tag: %d\n", tag);
		assert(0);
		break;
	}
	// assemble the packet (TLV format)
	getTLV(mem->send_buf, &mem->send_buf_len, tag, len, mem->receiverID, data);
}

LIFT_RESULT make_command_response_packet(uint8_t *rcv_data, Memory* mem){
	uint8_t response_packet[MAX_DATA_LENGTH] = {0};
	uint16_t cmd_type=0;
	uint16_t cmd=0;
	uint16_t seq_num=0;
	uint8_t plaintext[MIN_PACKET_LENGTH]={0};
	//uint8_t ciphertext[MIN_PACKET_LENGTH]={0};
	uint8_t response_payload[MAX_DATA_LENGTH]={0};
	uint8_t response[MIN_PACKET_LENGTH]={0};
	WORD_LEN plaintext_len=0;
	WORD_LEN response_len=0;
	WORD_LEN response_packet_len=0;

	//WORD i = 0;
	uint8_t valid = 0;
	//uint16_t start = 0;
	//uint16_t len = 0;

	//usleep(100);
	valid = make_decrypted_message(plaintext, &plaintext_len, mem,rcv_data );
	if (!valid) return RETURN_INVALID;

	struct lift_cmd_pkt *pkt = (struct lift_cmd_pkt *) plaintext;
	seq_num = pkt->seq_num;
	cmd_type = pkt->cmd_type;
	cmd = pkt->cmd;
	printf("Received: sequence number: [%d] cmd_type [%d]  cmd [%d] \n", seq_num,cmd_type,cmd);

	// depending on which is the current STATE, the according packet is constructed
	switch (cmd_type){
	case SESSION_CONTROL_REQUEST:
		if(cmd==DRONE_TURN_RIGHT)printf("DRONE TURNING RIGHT 1m... \n");
		else if(cmd==DRONE_TURN_LEFT)printf("DRONE TURNING LEFT 1m...\n");
		else if(cmd==DRONE_HIGHER)printf("DRONE HIGHER by 1m...\n");
		else if(cmd==DRONE_LOWER)printf("DRONE LOWER by 1m...\n");
		response_len=COMMAND_LENGTH;
		response[0]=SESSION_ACK;
		response[1]=SESSION_ACK >> 8;
		break;
	case SESSION_STATUS_REQUEST_MESSAGE:

		if (cmd==DRONE_GET_GPS){
			get_gps_coordinates(response);
			response_len=GPS_LENGTH;
		}
		else if (cmd==DRONE_GET_BATTERY){
			get_battery(response);
			response_len=COMMAND_LENGTH;
		}
		else if(cmd==DRONE_GET_TEMPERATURE){
			get_temperature(response);
			response_len=COMMAND_LENGTH;
		}
		else{
			return RETURN_INVALID; //do nothing, ignore the packet
		}

		break;
	case SESSION_VIDEO_STREAM_REQUEST:
		printf("--------------------STREAMING VIDEO IN THE BACKGROUND---------------------\n");
		response_len=COMMAND_LENGTH;
		response[0]=SESSION_ACK;
		response[1]=SESSION_ACK >> 8;  //start video threads
		mem->is_videostreaming=1;
		mem->current_state=STS_COMPLETED_DRONE;
		pthread_create(&gvideostream_handler, NULL, video_streaming_thread, (void*)mem);
		pthread_detach(gvideostream_handler);

		break;
	case SESSION_TERMINATE_VIDEO_STREAM:    // OK message, this massage does not contain any data
		printf("--------------------STOPPED STREAMING VIDEO -------------------------------\n");
		//len = 0;
		mem->is_videostreaming=0;
		response_len=COMMAND_LENGTH;
		response[0]=SESSION_ACK;
		response[1]=SESSION_ACK >> 8; //send rack //terminate video thread

		break;
	default:
		printf("problem here: a make function is not defined for this STATE (%s)\n", __func__);
		printf("tag: %d\n", cmd_type);
		assert(0);
		return RETURN_INVALID;
	}
	compose_command_response(response_payload,seq_num, cmd_type, cmd, response, response_len);
	response_packet_len=make_encrypted_message(response_packet,mem, response_payload, MIN_PACKET_LENGTH);

	// assemble the packet (TLV format)
	getTLV(mem->send_buf, &mem->send_buf_len, TAG_COMMAND, response_packet_len, mem->receiverID, response_packet);
	return RETURN_SUCCESS;
}



/* idle STATE of the drone, its FSM always starts here */
STATE idle_cc_handler( Memory* mem){
	PRINT_STATE()
	return STS_MAKE_0;
}

STATE sts_make_0_handler( Memory* mem){
	PRINT_STATE()
    startTimer(&mem->myTimer);
	make_packet(mem, TAG_STS_0);
	return STS_SEND_0;
}

STATE sts_make_1_handler( Memory* mem){
	PRINT_STATE()
    make_packet(mem, TAG_STS_1);
	return STS_SEND_1;
}

STATE sts_make_2_handler(Memory* mem){
	PRINT_STATE()
    make_packet(mem, TAG_STS_2);
	return STS_SEND_2;
}


/*  Here we wait for STS0 to arrive.
    Based on the validity of the message, the next STATE is defined */
STATE idle_drone_handler(Memory* mem){
	PRINT_STATE()
	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint16_t timeoutSocket = 0;
	uint16_t valid = 0;

	WORD_LEN len = 0;
	uint32_t crc = 0;
	WORD_TAG tag = TAG_UNDEFINED;
	uint8_t restart=0;
	while(1){
		initArray8(data, MAX_DATA_LENGTH);
		valid = receive_packet( &tag, &len, &crc, data, mem->receiverID, &timeoutSocket);
		if(timeoutSocket){
			// if timeout, just continue listening
		} else if(!valid){
			DEBUG_FSM("invalid id or incorrect integrity check")
		} else{
			//DEBUG_FSM("valid id and correct integrity check")
			if(tag == TAG_STS_0){

				restart = 1;
				if(crc == mem->STS0_CRC){  // compare STS_0 message with the one in memory
					restart = 0;
				}
				if(restart){
					if(DEBUG)printf("\treceive STS_0, start protocol; crc=[%08x]\n",crc);
					mem->STS0_CRC=crc;
					verify_STS_0(data, mem);
					return STS_MAKE_1;
				} else{
					if(DEBUG)printf("\treceive same STS_0, ignore; crc=[%08x]\n",crc);
				}
			}
			else{
				DEBUG_FSM("received unexpected tag, message ignored")
			}
		}
	}
}


/*  Here we wait for STS1 to arrive.
    Based on the validity of the message, the next STATE is defined */
STATE sts_send_0_handler(Memory* mem){

	PRINT_STATE()
	uint16_t valid = 0;
	WORD_TAG tag = TAG_UNDEFINED;
	WORD_LEN len = 0;
	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint16_t timeoutSocket = 0;
	uint16_t counter = 0;
	Timer myTimer;
	startTimer(&myTimer);

	send_packet(mem->send_buf, mem->send_buf_len);  // send packet
	DEBUG_FSM("send STS 0")

	for(counter = 0; counter < MAX_TRIALS; counter ++){

		startTimer(&myTimer);                           // start timer
		uint32_t crc = 0;
		while(valueTimer(&myTimer)<TIMEOUT){
			initArray8(data, MAX_DATA_LENGTH);
			valid = receive_packet( &tag, &len,&crc, data, mem->receiverID, &timeoutSocket);

			if(timeoutSocket){
				// timeout of UDP socket
			} else if(!valid){
				DEBUG_FSM("invalid id or incorrect integrity check")
			} else{

				if(tag == TAG_STS_1){
					if(DEBUG)printf("\tSTS 1 received crc=[%08x]\n",crc);
					valid = verify_STS_1(data, mem);
					if(valid){
						DEBUG_FSM("valid signature, send next message")
						return STS_MAKE_2;
					} else{
						DEBUG_FSM("invalid signature, continue listening")
					}
				} else{
					DEBUG_FSM("received unexpected tag, message ignored")
				}
			}
		}
		DEBUG_FSM("resend STS 0 (timeout)")
		send_packet(mem->send_buf, mem->send_buf_len);  // send packet
	}
	DEBUG_FSM("timeout and retransmission counter exceeded, restart protocol")
	return STS_MAKE_0;
}

/*  Here we wait for STS2 to arrive.
    Based on the validity of the message, the next STATE is defined */
STATE sts_send_1_handler(Memory* mem){
	PRINT_STATE()
	uint16_t valid = 0;
	WORD_TAG tag = TAG_UNDEFINED;
	WORD_LEN len = 0;
	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint16_t timeoutSocket = 0;
	Timer myTimer;

	send_packet(mem->send_buf, mem->send_buf_len); // send message stored in memory initially send sts1, then replace it by sts_ok after receiving sts2
	DEBUG_FSM("send STS 1")

	while(1){

		startTimer(&myTimer);       // start timer
		uint32_t crc = 0;
		while(valueTimer(&myTimer)<TIMEOUT){// why 5000?< here
			initArray8(data, MAX_DATA_LENGTH);
			valid = receive_packet( &tag, &len, &crc, data, mem->receiverID, &timeoutSocket);

			if(timeoutSocket){
				// timeout of UDP socket
			} else if(!valid){
				DEBUG_FSM("invalid id or incorrect integrity check")
			} else{
				if(tag == TAG_STS_2){
					if(DEBUG)printf("\tSTS 2 received crc=[%08x]\n",crc);
					valid = verify_STS_2(data, mem);
					if(valid){
						DEBUG_FSM("signature valid")
						make_packet(mem, TAG_STS_OK);
						send_packet(mem->send_buf, mem->send_buf_len);  // send packet
						printf("\tSending STS OK \n");
						printf("Key Establishment Phase Successful\n\n");
						return STS_COMPLETED_DRONE;
						//below is only for testing
						//if(INFINITE_LOOP_DSTS){
						//	return STS_COMPLETED_DRONE;
						//} else{
						//	return STATE_EXIT;
						// }
					} else{
						DEBUG_FSM("invalid signature, continue listening")
					}
				} else if(tag == TAG_STS_0){
					if(crc == mem->STS0_CRC){  // compare STS_0 message with the one in memory
						if(DEBUG)printf("\treceive same STS_0, ignore; crc=[%08x]\n",crc);
					} else{
						if(DEBUG)printf("\treceive new STS_0, restart protocol; crc=[%08x]\n",crc);
						mem->STS0_CRC=crc;
						verify_STS_0(data, mem);
						return STS_MAKE_1;
					}
				}
			}
		}
		DEBUG_FSM("Resend STS 1 (timeout)")
		send_packet(mem->send_buf, mem->send_buf_len);  // send packet
	}
}

STATE sts_send_2_handler(Memory* mem){
	PRINT_STATE()
	uint16_t valid = 0;
	WORD_TAG tag = TAG_UNDEFINED;
	WORD_LEN len = 0;
	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint16_t timeoutSocket = 0;
	uint16_t counter = 0;
	uint32_t time = 0;
	Timer myTimer;

	send_packet(mem->send_buf, mem->send_buf_len);  // send packet
	DEBUG_FSM("send STS 2")

	for(counter = 0; counter < MAX_TRIALS; counter ++){

		startTimer(&myTimer);                           // start timer
		uint32_t crc = 0;
		while(valueTimer(&myTimer)<TIMEOUT){
			initArray8(data, MAX_DATA_LENGTH);
			valid = receive_packet( &tag, &len, &crc, data, mem->receiverID, &timeoutSocket);

			if(timeoutSocket){
				// timeout of UDP socket
			} else if(!valid){
				DEBUG_FSM("invalid id or incorrect integrity check")
			} else{
				if(tag == TAG_STS_OK){
					if(DEBUG)printf("\tSTS OK received crc=[%08x]\n",crc);
					if(INFINITE_LOOP_STS){ //only for simulation
						mem->counter++;
						time = valueTimer(&mem->myTimer);
						if(mem->mean == 0){
							mem->mean = time;
						} else{
							mem->mean = (mem->mean*mem->counter + time)/(mem->counter + 1);
						}
						printf("\n==========================================================================================================\n");
						printTimer(&mem->myTimer);
						printf("\n\nstart test Nr %d %d %d\n\n\n", time,mem->mean, mem->counter);
						return IDLE_CC;
					}
					else{
						return STATE_EXIT;
					}
				} else{
					DEBUG_FSM("received unexpected tag, message ignored")
				}
			}
		}
		DEBUG_FSM("resend STS 2 (timeout)")
		send_packet(mem->send_buf, mem->send_buf_len);  // send packet
	}
	DEBUG_FSM("timout and retransmission counter exceeded, restart protocol")
	return STS_MAKE_0;
}



STATE sts_completed_drone_handler(Memory* mem){
	PRINT_STATE()
	WORD_TAG tag = TAG_UNDEFINED;
	WORD_LEN len = 0;
	uint8_t data[MAX_DATA_LENGTH] = {0};
	uint16_t timeoutSocket = 0;
	uint16_t valid = 0;
	uint16_t restart = 0;
	uint32_t crc = 0;


	// now the drone enters a loop that it can only exit if it receives STS 0 (restart protocol) or a communication message
	// if it receives a correct STS_2 message, it answers with an ok message
	while(1){
		initArray8(data, MAX_DATA_LENGTH);
		valid = receive_packet( &tag, &len, &crc,data,  mem->receiverID, &timeoutSocket);

		if(timeoutSocket){
			// do nothing
		} else if(!valid){
			DEBUG_FSM("invalid id or incorrect integrity check")
		} else{
			switch (tag)
			{
			case TAG_STS_0:
				restart = 1;
				if(crc == mem->STS0_CRC){  // compare STS_0 message with the one in memory
					restart = 0;
				}
				if(restart){
					if(DEBUG)printf("\tReceive new STS_0, restart protocol; crc=[%08x]\n",crc);
					mem->STS0_CRC = crc;
					verify_STS_0(data, mem); // process

					return STS_MAKE_1;
				} else{
					if(DEBUG)printf("receive STS_0 again, ignore; crc=[%08x]\n",crc);
				}
				break;
			case TAG_STS_2:
				if(DEBUG)printf("\tSTS 2 received again; crc=[%08x]\n",crc);
				valid = verify_STS_2(data, mem);
				if(valid){
					printf("\tSending STS OK \n");
					make_packet(mem, TAG_STS_OK);
					send_packet(mem->send_buf, mem->send_buf_len);
				} else{
					DEBUG_FSM("invalid signature for STS 2, don't send OK")
				}
				break;
			case TAG_COMMAND:
				if (crc!=mem->cmd_crc){
					if(DEBUG)printf("-----------------------------------------------------------------\n");
					if(DEBUG)printf("COMMAND_PACKET received; crc=[%08x]\n",crc);
					//not retransmitted data, so construct a new response packet
					valid = make_command_response_packet(data, mem);
					if (!valid)break;
					mem->cmd_crc=crc;
				}
				else printf("Received retransmitted packet, sending re-acknowledgment; crc=[%08x]\n",crc);
				send_packet(mem->send_buf, mem->send_buf_len); //send the response (new or retransmit) and wait for new packet to arrive
				break;
			default:
				DEBUG_FSM("received unexpected tag, message ignored")
				break;
			}
		}
	}
}





/* STS_0_data = session_control_public.x||session_control_public.y  (both coordinates are 256 bits => total of 526 bits of data )*/
void make_STS_0_data(uint8_t *data, Memory* mem, WORD_LEN *len){     

	WORD* privateKey        = mem->session_control_secret; // this is to make life (and writing) easier, privateKey and publicKey are the private and public key stored in the memory
	p256_affine* publicKey  = &mem->session_control_public;
	WORD i                  = 0;
	uint8_t pointx8[P256_POINT_LEN] = {0};
	uint8_t pointy8[P256_POINT_LEN] = {0};

	initArray8(data, MAX_DATA_LENGTH); // to be sure everything is at zero
	initArray(publicKey->x, SIZE);
	initArray(publicKey->y, SIZE);
	initArray(privateKey, SIZE);

	// 1. compute private key
	random_gen(privateKey, SIZE_EC_KEY, &mem->pool);

	// 2. compute public key: session_control_public = c*G
	pointScalarMultAffineWord(publicKey, &mem->G, privateKey);

	// 3. assemble payload
	word2rawbyte(pointx8, publicKey->x, P256_POINT_LEN);
	word2rawbyte(pointy8, publicKey->y, P256_POINT_LEN);
	for(i=0;i<P256_POINT_LEN;i++){
		data[i]             = pointx8[i];
		data[i+P256_POINT_LEN]   = pointy8[i];
	}

	// 4. set data payload length
	*len=2*P256_POINT_LEN;

	// 5. print result
	if(DEBUG_SIGNATURE){
		printf("control center's private key        :"); print_hex_type(privateKey,16);
		printf("control center's public key.x-----  :");print_hex_type(publicKey->x,16);
		printf("control center's public key.y-----  :");print_hex_type(publicKey->y,16);
		printf("data x                      -----  :");print_array(publicKey->x,SIZE);
		printf("data y                      -----  :");print_array(publicKey->y,SIZE);
		printf("data x                      -----  :");print_array8(pointx8,P256_POINT_LEN);
		printf("data y                      -----  :");print_array8(pointy8,P256_POINT_LEN);
		printf("data                       -----  :");print_array8(data,2*P256_POINT_LEN);
	}

}

/* STS_1_data = session_drone_public.x||session_control_public.x||Encryption[signature(session_drone_public.x||session_drone_public.x||session_control_public.x||session_control_public.x)] */
void make_STS_1_data(uint8_t *data, Memory* mem, WORD_LEN *len){  

	uint8_t signature8[SIG_LEN]         = {0};
	uint8_t pointx8[P256_POINT_LEN]          = {0};
	uint8_t pointy8[P256_POINT_LEN]          = {0};
	uint8_t nonce[CHACHA_NONCE_LENGTH]  = {0};
	uint8_t mac_tag[MAC_TAG_LENGTH]     = {0};
	uint8_t ciphertext[SIG_LEN]         = {0};
	uint16_t start                      = 0;

	WORD i                              = 0;
	WORD signature[SIZE]                = {0};
	WORD rand[SIZE]                     = {0};
	WORD toBeSigned[P256_POINT_LEN/BYTE*4+1] = {0};
	WORD*           drone_private_key   = mem->session_drone_secret; // private session key of drone
	p256_affine*    drone_public_key    = &mem->session_drone_public; // public session key of drone
	p256_affine*    control_public_key  = &mem->session_control_public; // public session key of drone
	p256_affine     session_key_affine  = {{0}};

	// to make all arrays that will we written at are set to zero
	initArray8(data, MAX_DATA_LENGTH);
	initArray(drone_public_key->x, SIZE);
	initArray(drone_public_key->y, SIZE);
	initArray(drone_private_key, SIZE);
	initArray(mem->session_key, SIZE);
	initArray8(mem->session_key8, CHACHA_KEY_LENGTH);

	// 1. generate drone's private key, which is a random_gen number
	random_gen(drone_private_key, SIZE_EC_KEY, &mem->pool);
	//convert(drone_private_key, "c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357");

	// 2. generate drone's public key (compute session_drone_public = d*G)
	pointScalarMultAffineWord(drone_public_key, &mem->G, drone_private_key);
	word2rawbyte(pointx8, drone_public_key->x, P256_POINT_LEN);// keep an 8-bit version for putting it in the data packet
	word2rawbyte(pointy8, drone_public_key->y, P256_POINT_LEN);

	// 3. create the signature (mind de fact that i begins at 1 to exclude the length field!)
	for(i=1;i<=drone_public_key->x[0];i++){
		toBeSigned[i+0*P256_POINT_LEN/BYTE]  = drone_public_key->x[i];
	}
	for(i=1;i<=drone_public_key->y[0];i++){
		toBeSigned[i+1*P256_POINT_LEN/BYTE]  = drone_public_key->y[i];
	}
	for(i=1;i<=control_public_key->x[0];i++){
		toBeSigned[i+2*P256_POINT_LEN/BYTE]  = control_public_key->x[i];
	}
	for(i=1;i<=control_public_key->y[0];i++){
		toBeSigned[i+3*P256_POINT_LEN/BYTE]  = control_public_key->y[i];
	}
	toBeSigned[0] = 4*P256_POINT_LEN/BYTE;
	signature_gen(signature,mem->drone_SK,toBeSigned, mem->N, mem->G.x, mem->G.y);

	// 4. compute the session key k = H( (da*Qb).x ) and store it in memory
	pointScalarMultAffineWord(&session_key_affine, control_public_key, mem->session_drone_secret);  // compute session point
	hash(mem->session_key,session_key_affine.x,SIZEHASH);                                   //compute session key by taking hash of session key x coordinate

	// 5. generate the nonce
	//char2byte(nonce, "4041424344454647"); // should be changed to random_gen number
	random_gen(rand, CHACHA_NONCE_LENGTH*8, &mem->pool);
	word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);

	// 6. encrypt the sigature + compute the mac. Everything must be translated in uint8_t
	word2rawbyte(signature8, signature, SIG_LEN);// keep an 8-bit version for the encryption
	word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH);                   // keep an 8-bit version for the encryption
	aead_chacha20_poly1305(mac_tag,ciphertext, mem->session_key8, 32, nonce, signature8, SIG_LEN); //encrypt the signature with the session key k

	// 7. merge everything together
	start = 0;
	for(i=0;i<P256_POINT_LEN;i++){
		data[i]             = pointx8[i];
		data[i+P256_POINT_LEN]   = pointy8[i];
	}
	start += 2*P256_POINT_LEN;
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
	assert((start == 2*P256_POINT_LEN + SIG_LEN + MAC_TAG_LENGTH + CHACHA_NONCE_LENGTH)); // verification

	// 8. print
	if(DEBUG_SIGNATURE){
		printf("cc's secret  (unknown normally stays at 0): ");    print_num(mem->session_control_secret);
		printf("cc's point x (retrieved): ");    print_num(mem->session_control_public.x);
		printf("cc's point y (retrieved): ");    print_num(mem->session_control_public.y);
		printf("\n");
		printf("drone's secret  : ");    print_num(mem->session_drone_secret);
		printf("drone's point x : ");    print_num(mem->session_drone_public.x);
		printf("drone's point y : ");    print_num(mem->session_drone_public.y);
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

/* STS_2_data = Encryption[signature(session_control_public.x||session_control_public.y||session_drone_public.x||session_drone_public.y)] */
void make_STS_2_data(uint8_t *data, Memory* mem, WORD_LEN *len){   

	p256_affine session_key_affine = {{0}};
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
	for(i=1;i<=mem->session_control_public.x[0];i++){
		toBeSigned[i+0*P256_POINT_LEN/BYTE]  = mem->session_control_public.x[i];
	}
	for(i=1;i<=mem->session_control_public.y[0];i++){
		toBeSigned[i+1*P256_POINT_LEN/BYTE]  = mem->session_control_public.y[i];
	}
	for(i=1;i<=mem->session_drone_public.x[0];i++){
		toBeSigned[i+2*P256_POINT_LEN/BYTE]  = mem->session_drone_public.x[i];
	}
	for(i=1;i<=mem->session_drone_public.y[0];i++){
		toBeSigned[i+3*P256_POINT_LEN/BYTE]  = mem->session_drone_public.y[i];
	}
	toBeSigned[0] = 4*P256_POINT_LEN/BYTE;
	signature_gen(signature,mem->control_SK,toBeSigned, mem->N, mem->G.x, mem->G.y);

	// 2. compute session key and store it into memory
	pointScalarMultAffineWord(&session_key_affine, &mem->session_drone_public, mem->session_control_secret);     // compute session point
	hash(mem->session_key,session_key_affine.x,SIZEHASH);    //compute session key by taking hash of session key x coordinate

	// 3. generate the nonce
	//char2byte(nonce, "4041424344454647"); // should be changed to random_gen number
	random_gen(rand, CHACHA_NONCE_LENGTH*8, &mem->pool);
	word2rawbyte(nonce, rand, CHACHA_NONCE_LENGTH);

	// 4. encrypt the signature + sign it. Everything has to be set to uint8_t
	word2rawbyte(signature8, signature,SIG_LEN);
	word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH); // convert to uint8
	aead_chacha20_poly1305(mac_tag,ciphertext, mem->session_key8, CHACHA_KEY_LENGTH, nonce, signature8, SIG_LEN); //encrypt the signature with the session key k

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
	assert((*len = CHACHA_NONCE_LENGTH + SIG_LEN + MAC_TAG_LENGTH)); // check

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



/* rcv_data = STS_0_data = session_control_public.x||session_control_public.y */
void verify_STS_0(uint8_t *rcv_data, Memory* mem){  

	WORD i = 0;
	uint8_t pointx8[P256_POINT_LEN] = {0};
	uint8_t pointy8[P256_POINT_LEN] = {0};

	initArray(mem->session_control_public.x, SIZE);
	initArray(mem->session_control_public.y, SIZE);

	// 1.retreive control center's public key from STS_0 message, and store it in memory
	for(i=0;i<P256_POINT_LEN;i++){
		pointx8[i] = rcv_data[i];
		pointy8[i] = rcv_data[i+P256_POINT_LEN];
	}
	rawbyte2word(mem->session_control_public.x,pointx8, P256_POINT_LEN); // transform array of bytes without length field into an array of words with a length field
	rawbyte2word(mem->session_control_public.y,pointy8, P256_POINT_LEN);

	// 3. prints
	if(DEBUG_SIGNATURE){
		printf("received cc's point x : ");    print_num(mem->session_control_public.x);
		printf("received cc's point y : ");    print_num(mem->session_control_public.y);
	}
}

/* rcv_data = STS_1_data = session_drone_public.x||session_drone_public.y||nonce||Encryption[signature(session_drone_public.x||session_drone_public.y||session_control_public.x||session_control_public.y)]||MAC */
LIFT_RESULT verify_STS_1(uint8_t *rcv_data, Memory* mem){

	WORD i                          = 0;
	WORD start                      = 0;
	uint16_t valid                  = 1;
	WORD signature[SIZE]            = {0};
	WORD toBeSigned[SIZE]           = {0};
	uint8_t signature8[SIG_LEN]     = {0};
	uint8_t rcv_pointx[P256_POINT_LEN]  = {0};
	uint8_t rcv_pointy[P256_POINT_LEN]  = {0};

	uint8_t nonce[CHACHA_NONCE_LENGTH]={0};
	uint8_t mac_tag[MAC_TAG_LENGTH] = {0};
	uint8_t ciphertext[SIG_LEN]={0};
	p256_affine     session_key_affine  = {{0}};

	initArray(mem->session_drone_public.x, SIZE);
	initArray(mem->session_drone_public.y, SIZE);
	initArray(mem->session_key, SIZE);
	initArray8(mem->session_key8, CHACHA_KEY_LENGTH);

	// 1. retrieve data (points + signature) and store them in memory
	start = 0;
	for(i = 0; i<P256_POINT_LEN; i++){
		rcv_pointx[i] = rcv_data[i];
		rcv_pointy[i] = rcv_data[i+P256_POINT_LEN];
	}
	start += 2*P256_POINT_LEN;
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

	rawbyte2word(mem->session_drone_public.x, rcv_pointx, P256_POINT_LEN); // store in memory
	rawbyte2word(mem->session_drone_public.y, rcv_pointy, P256_POINT_LEN);


	// 2. compute key
	pointScalarMultAffineWord(&session_key_affine, &mem->session_drone_public, mem->session_control_secret);     // compute session point
	hash(mem->session_key,session_key_affine.x,SIZEHASH);    //compute session key by taking hash of session key x coordinate

	// 3. verify mac
	word2rawbyte(mem->session_key8, mem->session_key, CHACHA_KEY_LENGTH);
	valid = verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN); //this is not working

	if(DEBUG_SIGNATURE){
		printf("[verify_STS_1_data] session_key x affine----- ");print_hex_type(session_key_affine.x,16);
		printf("[verify_STS_1_data] session_key y affine----- ");print_hex_type(session_key_affine.y,16);
		printf("[verify_STS_1_data] rcv drone's point x : ");    print_num(mem->session_drone_public.x);
		printf("[verify_STS_1_data] rcv drone's point y : ");    print_num(mem->session_drone_public.y);
		printf("[verify_STS_1_data] rcv nonce           : ");        print_array8(nonce, CHACHA_NONCE_LENGTH);
		printf("[verify_STS_1_data] computed key        : ");       print_num(mem->session_key);
		printf("[verify_STS_1_data] computed key 8      : ");       print_array8(mem->session_key8, CHACHA_KEY_LENGTH);
		printf("[verify_STS_1_data] rcv cipher          : ");       print_array8(ciphertext, SIG_LEN);
		printf("[verify_STS_1_data] rcv mac             : ");          print_array8(mac_tag, MAC_TAG_LENGTH);
	}
	if(!valid){
		return RETURN_INVALID;
	}

	// 4. decrypt
	encrypt_init(signature8,mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, 0);

	// 5. verify signature
	rawbyte2word(signature, signature8, SIG_LEN);
	for(i=1;i<=P256_POINT_LEN/BYTE;i++){
		toBeSigned[i]                   = mem->session_drone_public.x[i];
		toBeSigned[i+P256_POINT_LEN/BYTE]    = mem->session_drone_public.y[i];
		toBeSigned[i+2*P256_POINT_LEN/BYTE]  = mem->session_control_public.x[i];
		toBeSigned[i+3*P256_POINT_LEN/BYTE]  = mem->session_control_public.y[i];
	}
	toBeSigned[0] = 4*P256_POINT_LEN/BYTE;
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


/* rcv_data = STS_2_data = Encryption[signature(session_control_public.x||session_control_public.||session_drone_public.x||session_drone_public.y]||MAC */
LIFT_RESULT verify_STS_2(uint8_t *rcv_data, Memory* mem){    //rcv_data = STS_2_data = Encryption[signature(session_control_public.x||session_drone_public.x)]

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
	assert((len = CHACHA_NONCE_LENGTH + SIG_LEN + MAC_TAG_LENGTH)); // check


	// 2. verify mac
	valid = verify_mac_aead_chacha20_poly1305(mac_tag, mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN); //this is not working
	if(DEBUG_SIGNATURE){
		printf("drone's point x : ");    print_num(mem->session_drone_public.x);
		printf("drone's point y : ");    print_num(mem->session_drone_public.y);
		printf("cc's    point x : ");    print_num(mem->session_control_public.x);
		printf("cc's    point y : ");    print_num(mem->session_control_public.y);
		printf("\n");
		printf("[make_STS_2] session key: ");    print_num(mem->session_key);
		printf("[verify_STS_2] received nonce       : ");    print_array8(nonce, CHACHA_NONCE_LENGTH);
		printf("[verify_STS_2] received sig (cipher): ");    print_array8(ciphertext, SIG_LEN);
		printf("[verify_STS_2] received mac         : ");    print_array8(mac_tag, MAC_TAG_LENGTH);
	}

	if(!valid){
		return RETURN_INVALID;
	}

	// 3. decrypt
	encrypt_init(signature8,mem->session_key8, CHACHA_KEY_LENGTH, nonce, ciphertext, SIG_LEN, 0);
	rawbyte2word(signature, signature8, SIG_LEN);

	// 4. verify signature
	for(i=1;i<=P256_POINT_LEN/BYTE;i++){
		toBeSigned[i]                   = mem->session_control_public.x[i];
		toBeSigned[i+P256_POINT_LEN/BYTE]    = mem->session_control_public.y[i];
		toBeSigned[i+2*P256_POINT_LEN/BYTE]  = mem->session_drone_public.x[i];
		toBeSigned[i+3*P256_POINT_LEN/BYTE]  = mem->session_drone_public.y[i];
	}
	toBeSigned[0] = 4*P256_POINT_LEN/BYTE;
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
