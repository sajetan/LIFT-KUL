/*
 * fsm.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef FSM_H_
#define FSM_H_

#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include "../config.h"
#include "globals.h"
#include "utilities.h"
#include "message.h"    // for WORD_ID
#include "communication.h"
#include "../ecc/p256.h"
#include "../random/random_gen.h"
#include "../hash/hash.h"
#include "../ecc/signature.h"
#include "../aead/chacha20_poly1305_interface.h"
#include "../drone/drone_includes.h"
#include "../control/control_includes.h"


#define MAX_TRIALS 10
#define MAX_COMMUNICATION_RETRANSMISSION 20 //number of retransmission of communication packets
#define SESSION_TIMEOUT 6000000 //sixty minutes in milliseconds, can be configures as required
#define COMMUNICATION_TIMEOUT 1000 //retransmit after one second

// parameters of the fsm
#define INFINITE_LOOP_STS 0     // 1: run STS infinitely, 0: run STS once and exit (doesn't really work yet, since both must know the other exits)
#define INFINITE_LOOP_DSTS 1     // 1: run STS infinitely, 0: run STS once and exit (doesn't really work yet, since both must know the other exits)
#define TIMEOUT 400            // in ms, timeout value. /!\ not the same as the socket timeout, which has been set to a very small number

#define COMMAND_LENGTH 2 //in bytes
#define SEQ_LENGTH 4 //in bytes
#define GPS_LENGTH 16 //in bytes, 8 for lattitude, 8 for longitude
#define VIDEOFRAME_LENGTH (VIDEOFRAMES*32+4)

typedef enum
{
	NULL_STATE,
	IDLE_DRONE,
	IDLE_CC,
	STS_SEND_0,
	STS_SEND_1,
	STS_SEND_2,
	STS_MAKE_0,
	STS_MAKE_1,
	STS_MAKE_2,
	STS_COMPLETED_DRONE,
	CONTROL_SEND_COMMAND,
	DRONE_PROCESS_COMMAND,
	STATE_EXIT,
	DRONE_UNREACHABLE,
	RESTART_SESSION,
	SESSION_TIMEOUT_STATE
} STATE;


typedef enum{
	SESSION_CONTROL_REQUEST=1,
	SESSION_STATUS_REQUEST_MESSAGE,
	SESSION_VIDEO_STREAM_REQUEST,
	SESSION_TERMINATE_VIDEO_STREAM,
}SESSION_COMMAND_TYPE;


typedef enum{
	INIT_ESTABLISH_CONNECTION=1,
	START_COMMUNICATION_STATUS,
	START_COMMUNICATION_COMMAND,
	START_VIDEO_STREAM_COMMUNICATION,
	TERMINATE_VIDEO_STREAM_COMMUNICATION,
	REINIT_ESTABLISH_CONNECTION,
	TERMINATE_SYSTEM=9
}SESSION_ACTIONS;

typedef enum{
	NULL_COMMAND,
	DRONE_TURN_RIGHT,
	DRONE_TURN_LEFT,
	DRONE_HIGHER,
	DRONE_LOWER,
	DRONE_VIDEO_FRAME,
	SESSION_ACK,
	SESSION_REACK
}CONTROL_COMMAND;

typedef enum{
	DRONE_GET_GPS=1,
	DRONE_GET_BATTERY,
	DRONE_GET_TEMPERATURE
}STATUS_COMMAND;


typedef struct Memory Memory;
struct Memory
{
	WORD_ID receiverID;
	EntropyPool pool;

	// parameters used in caculations
	p256_affine G;
	WORD N[SIZE];

	// communication specific
	uint8_t send_buf[MAX_TRANSFER_LENGTH];
	uint16_t send_buf_len;
	uint8_t send_vidbuf[MAX_TRANSFER_LENGTH];
	uint16_t send_vidbuf_len;

	uint32_t STS0_CRC; //keeping track of previous STS0 message

	// permanent public and private key pairs
	p256_affine drone_PK;
	p256_affine control_PK;
	WORD drone_SK[SIZE];
	WORD control_SK[SIZE];

	// session specific : these are the public and secret keys for 1 session
	WORD        session_drone_secret[SIZE];         // secret key used to generate the public key
	WORD        session_control_secret[SIZE];       // secret key used to generate the public key
	p256_affine session_control_public;                     // first point computed by CC
	p256_affine session_drone_public;                     // second point computed by drone
	WORD        session_key[SIZE];          // session key
	uint8_t     session_key8[CHACHA_KEY_LENGTH]; // session key in uint18_t form

	// debugging
	uint32_t  counter;  // counter for counter the number of iterations
	uint32_t  mean;  // counter for counter the number of iterations
	Timer myTimer;

	//communication related fields
	uint32_t volatile seq_num;
	uint32_t volatile vid_seq_num;
	uint32_t volatile vid_count;
	uint32_t volatile vid_seq_num_start;
	uint32_t volatile cmd_crc;
	uint32_t volatile cmd;
	uint32_t volatile cmd_type;
	uint8_t volatile is_videostreaming; //0: not streaming, 1: streaming
	uint8_t volatile current_state;

};

//void initMemory(Memory* mem);
void initMemory(Memory* mem, uint8_t isDrone);

void initDroneMemory(Memory* mem);
void initCCMemory(Memory* mem);


// make, send and receive functions
void make_packet(Memory* mem, WORD_TAG tag);
void send_packet(uint8_t* buf, uint16_t buf_len);
LIFT_RESULT receive_packet(WORD_TAG *tag, WORD_LEN *len,uint32_t *crc, uint8_t* data, WORD_ID id, uint16_t* timeout);

// next STATE logic functions
STATE idle_cc_handler(Memory* mem);
STATE idle_drone_handler(Memory* mem);


STATE sts_make_0_handler(Memory* mem);
STATE sts_make_1_handler(Memory* mem);
STATE sts_make_2_handler(Memory* mem);

STATE sts_send_0_handler(Memory* mem);
STATE sts_send_1_handler(Memory* mem);
STATE sts_send_2_handler(Memory* mem);

STATE sts_completed_drone_handler(Memory* mem);


// STS specific functions
void make_STS_0_data(uint8_t *data, Memory* mem, WORD_LEN *len);
void make_STS_1_data(uint8_t *data, Memory* mem, WORD_LEN *len);
void make_STS_2_data(uint8_t *data, Memory* mem, WORD_LEN *len);

void     verify_STS_0(uint8_t *rcv_data, Memory* mem);
LIFT_RESULT verify_STS_1(uint8_t *rcv_data, Memory* mem);
LIFT_RESULT verify_STS_2(uint8_t *rcv_data, Memory* mem);

//new functions
uint16_t make_encrypted_message(uint8_t* data, Memory* mem, uint8_t* plaintext, WORD_LEN plaintext_len);
LIFT_RESULT make_decrypted_message(uint8_t* plaintext, WORD_LEN* plaintext_len, Memory* mem, uint8_t* rcv_data);
LIFT_RESULT make_command_request_packet(Memory* mem);
LIFT_RESULT make_command_response_packet(uint8_t *data, Memory* mem);
void compose_command_response(uint8_t* response_packet,uint32_t seq_num,  WORD_TAG cmd_type,  WORD_LEN cmd, uint8_t* response, WORD_LEN response_length);
void decompose_command_response(uint8_t *data, uint32_t *seq_num,  WORD_TAG *cmd_type,  WORD_LEN *cmd, uint8_t* response, WORD_LEN response_len);


#endif
