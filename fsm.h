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
#include"message.h"    // for WORD_ID
#include"communication.h"

typedef enum
{
    STS_drone_listening,
    STS_send_0,
    STS_send_1,
    STS_send_2,
    STS_drone_completed,
    STS_CC_completed,
    State_Exit
} State;

typedef struct Memory Memory;
struct Memory
{
    WORD_ID receiverID;
};

void initMemory(Memory* mem);
State STS_send_0_fct(uint8_t* buf, uint16_t* buf_len, Memory* memory);
State STS_send_1_fct(uint8_t* buf, uint16_t* buf_len, Memory* memory);
State STS_send_2_fct(uint8_t*, uint16_t* buf_len, Memory* memory);
State STS_CC_completed_fct();
State STS_drone_completed_fct();


#endif