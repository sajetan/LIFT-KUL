 /*
 * second draft of the FSM 
 * fsm.h
 * 
 *
 *  Created on: April 18, 2020
 *      Author: Ferdinand Hannequart
 */
 
#ifndef FSM_H_
#define FSM_H_
//#include"communication.h"     // for MAX_TRANSFER_LENGTH

#include "globals.h"
#include "utilities.h"
#include "message.h"    // for WORD_ID
#include "communication.h"
#include "p256.h"
#include "random.h"
#include "hash.h"
#include "signature.h"
#include "chacha20_poly1305_interface.h"

// all printing functions for debugging
#define PRINT_STATE()   if(1)\
                        printf("[ Current state:  %s ]\n", __func__);
#define DEBUG_FSM(s)    if(1)\
                        printf("\t%s \n",s);
#define DEBUG_SIGNATURE 0
#define PRINT_CONTENT_UDP_PACKET 0
#define MAX_TRIALS 3

// parameters of the fsm
#define INFINITE_LOOP_STS 1     // 1: run STS infinitely, 0: run STS once and exit (doesn't really work yet, since both must know the other exits)
#define TIMEOUT 200            // in ms, timeout value. /!\ not the same as the socket timeout, which has been set to a very small number
#define SIMULATE_PACKET_DROP 50 // percentage chance of a packet drop, vary this between 0 and 100

typedef enum
{
    idle_drone,
    idle_CC,
    key_exchange_CC,
    key_exchange_drone,
    STS_send_0,
    STS_send_1,
    STS_send_2,
    STS_make_0,
    STS_make_1,
    STS_make_2,
    STS_receive_0,
    STS_receive_1,
    STS_receive_2,
    STS_send_OK,
    STS_receive_OK,
    State_Exit
} State;

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
    uint8_t rcv_STS_0[MAX_DATA_LENGTH];
    uint16_t rcv_STS_0_len;
    uint16_t retransmissionCounter;
    
    // permanent public and private key pairs
    p256_affine drone_PK;
    p256_affine control_PK;
    WORD drone_SK[SIZE];
    WORD control_SK[SIZE];

    // session specific : these are the public and secret keys for 1 session
    WORD        drone_secret[SIZE];         // secret key used to generate the public key
    WORD        control_secret[SIZE];       // secret key used to generate the public key 
    p256_affine pointc;                     // first point computed by CC
    p256_affine pointd;                     // second point computed by drone
    WORD        session_key[SIZE];          // session key
    uint8_t     session_key8[CHACHA_KEY_LENGTH]; // session key in uint18_t form
    
    // debugging
    uint32_t  counter;  // counter for counter the number of iterations
    uint16_t print;

};

void initMemory(Memory* mem);

// make, send and receive functions
void make_packet(Memory* mem, WORD_TAG tag);
void send_packet(uint8_t* buf, uint16_t buf_len);
uint16_t receive_packet(WORD_TAG *tag, WORD_LEN *len, uint8_t* data, WORD_ID id, uint16_t* timeout);

// next state logic functions
State idle_CC_fct(Memory* mem);
State idle_drone_fct(Memory* mem);

State key_exchange_drone_fct(Memory* mem);
State key_exchange_CC_fct(Memory* mem);

State STS_make_0_fct(Memory* mem);
State STS_make_1_fct(Memory* mem);
State STS_make_2_fct(Memory* mem);

State STS_send_0_fct(Memory* mem);
State STS_send_1_fct(Memory* mem);
State STS_send_2_fct(Memory* mem);

State STS_receive_0_fct(Memory* mem);
State STS_receive_1_fct(Memory* mem);
State STS_receive_2_fct(Memory* mem);

State STS_send_OK_fct(Memory* mem);
State STS_receive_OK_fct(Memory* mem);



// STS specific functions
void make_STS_0_data(uint8_t *data, Memory* mem, WORD_LEN *len);
void make_STS_1_data(uint8_t *data, Memory* mem, WORD_LEN *len);
void make_STS_2_data(uint8_t *data, Memory* mem, WORD_LEN *len);

uint16_t verify_STS_0(uint8_t *rcv_data, Memory* mem);
uint16_t verify_STS_1(uint8_t *rcv_data, Memory* mem);
uint16_t verify_STS_2(uint8_t *rcv_data, Memory* mem);

#endif
