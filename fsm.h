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

#include"globals.h"
#include"utilities.h"
#include"message.h"    // for WORD_ID
#include"communication.h"
#include"p256.h"
#include"random.h"
#include"hash.h"

typedef enum
{
    STS_drone_listening,
    STS_send_0,
    STS_send_1,
    STS_send_2,
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
    WORD pk[SIZE];      // public key
    WORD sk[SIZE];      // secret key
    //p256_affine G;
};

void initMemory(Memory* mem, char* publicKey, char* secretKey);

State STS_send_0_fct(uint8_t* buf, Memory* memory);
State STS_send_1_fct(uint8_t* buf, Memory* memory);
State STS_send_2_fct(uint8_t*,  Memory* memory);
State STS_receive_0_fct(uint8_t* buf, Memory* mem);
State STS_receive_1_fct(uint8_t* buf, Memory* mem);
State STS_receive_2_fct(uint8_t* buf, Memory* mem);

State STS_send_OK_fct(uint8_t* buf, Memory* mem);
State STS_receive_OK_fct(uint8_t* buf,  Memory* mem);
State STS_CC_completed_fct();
State STS_drone_completed_fct();

void make_STS_0_data(uint8_t *data, Memory* mem);
void make_STS_1_data(uint8_t *data, uint8_t *rcv_data);
void make_STS_2_data(uint8_t *data, uint8_t *rcv_data);
uint8_t verify_STS_1(uint8_t *rcv_data);
uint8_t verify_STS_2(uint8_t *rcv_data);

#endif
