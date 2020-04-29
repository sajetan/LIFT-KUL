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
#include "chacha20_poly1305_interface.h"

#define S()     printf("\nCurrent state:  %s\n", __func__);

typedef enum
{
    idle_drone,
    idle_CC,
    STS_drone_listening,
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
    STS_drone_completed,
    STS_CC_completed,
    State_Exit
} State;

typedef struct Memory Memory;
struct Memory
{
    WORD_ID receiverID;
    EntropyPool pool;
    WORD PKCX[SIZE];
    WORD PKCY[SIZE]; // public key of control center
    WORD PKDX[SIZE];
    WORD PKDY[SIZE]; // public key of drone
    WORD GX[SIZE];
    WORD GY[SIZE]; // Generator parameters
    WORD N[SIZE]; //N parameter
    WORD SK[SIZE];     // secret key

    // communication specific
    WORD send_buf[MAX_TRANSFER_LENGTH];
    WORD rcv_buf[MAX_TRANSFER_LENGTH];
    WORD rcv_data[MAX_DATA_LENGTH];
    uint16_t send_buf_len;
    uint16_t rcv_buf_len;

    // session specific
    WORD point_C[SIZE];     // first point computed by CC
    WORD point_D[SIZE];     // second point computed by drone
    uint8_t SESSION_KEY[32]; // public key of drone

};

void initMemory(Memory* mem);


// make, send and receive functions
void make_packet(Memory* mem, State state);
void send_packet(Memory* mem);
uint16_t receive_packet(Memory* mem, WORD_TAG expectedTag);

// next state logic functions
State idle_CC_fct(Memory* mem);
State idle_drone_fct(Memory* mem);

State STS_make_0_fct(Memory* mem);
State STS_make_1_fct(Memory* mem);
State STS_make_2_fct(Memory* mem);

State STS_send_0_fct(Memory* mem);
State STS_send_1_fct(Memory* mem);
State STS_send_2_fct(Memory* memory);


State STS_receive_0_fct(Memory* mem);
State STS_receive_1_fct(Memory* mem);
State STS_receive_2_fct(Memory* mem);

State STS_send_OK_fct(uint8_t* buf, Memory* mem);
State STS_receive_OK_fct(uint8_t* buf,  Memory* mem);
State STS_CC_completed_fct();
State STS_drone_completed_fct();

void make_STS_0_data(uint8_t *data, Memory* mem, WORD *len);
void make_STS_1_data(uint8_t *data, uint8_t *rcv_data, Memory* mem, WORD *len);
void make_STS_2_data(uint8_t *data, uint8_t *rcv_data, Memory* mem, WORD *len);
uint8_t verify_STS_1(uint8_t *rcv_data, Memory* mem);
uint8_t verify_STS_2(uint8_t *rcv_data, Memory* mem);

#endif
