/*
 * message.h
 * 
 *
 *  Created on: April 9, 2020
 *      Author: Ferdinand Hannequart
 */

#ifndef DEF_MESSAGE
#define DEF_MESSAGE

#include"globals.h"
#include"communication.h"

//#include"main.h"

/*
    Format of STS message:
    type    :   [   Tag         Length      ID          ||  Data    ]
    bytes   :   [   BYTE_TAG    BYTE_LEN    BYTE_ID     ||  ?       ]
    bytes   :   [   1           2           8           ||  ?       ]
*/


#define WORD_TAG    uint8_t
#define WORD_LEN    uint8_t
#define WORD_ID     uint8_t

#define BYTE_TAG    sizeof(WORD_TAG)
#define BYTE_LEN    sizeof(WORD_LEN)
#define BYTE_ID     sizeof(WORD_ID)

#define MAX_DATA_LENGTH   (MAX_TRANSFER_LENGTH -  BYTE_TAG - BYTE_LEN - BYTE_ID)            // maximum number of bytes of data


#define TAG_STS_0   (WORD_TAG) 0
#define TAG_STS_1   (WORD_TAG) 1
#define TAG_STS_2   (WORD_TAG) 2
#define TAG_VIDEO   (WORD_TAG) 3
#define TAG_CONTROL (WORD_TAG) 4
#define TAG_SAY_HELLO_TO_DRONE  (WORD_TAG) 5
#define TAG_SAY_HELLO_TO_CC     (WORD_TAG) 6

/*
typedef struct Session Session;
struct User
{
    WORD pk[SIZE];          // public key
    WORD sessionKey[SIZE];  // session key
    WORD id[SIZE];          // identity of contact
    WORD seqNumContact;     // sequence number of messages sent by the contact
};

// define a structure for a party in the communication.
// it holds the users key pair, entropy pool and list of active sessions
typedef struct User User;
struct User
{
    WORD            pk[SIZE];                       // public key
    WORD            sk[SIZE];                       // secret key
    Session         sessions[MAX_NUMBER_SESSIONS];  // all sessions are listed here
    EntropyPool     pool;                           // entropy pool used for generating pseudorandom numbers
};
*/
void getTLV(uint8_t* buf, uint16_t* buf_len, WORD_TAG tag,  WORD_LEN dataLength, WORD_ID id, uint8_t* data);
void decomposeTLV( WORD_TAG* tag,  WORD_LEN* dataLength, WORD_ID* id, uint8_t* data, uint8_t* buf, uint16_t buf_len);
void getTLVTest();

#endif