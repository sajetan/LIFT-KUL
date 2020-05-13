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


#define WORD_TAG    uint16_t
#define WORD_LEN    uint16_t
#define WORD_ID     uint16_t

#define BYTE_TAG    sizeof(WORD_TAG)
#define BYTE_LEN    sizeof(WORD_LEN)
#define BYTE_ID     sizeof(WORD_ID)

#define MAX_DATA_LENGTH   (MAX_TRANSFER_LENGTH -  BYTE_TAG - BYTE_LEN - BYTE_ID)            // maximum number of bytes of data
#define MIN_PACKET_LENGTH   (128)            // min number of bytes of data payload length

#define TAG_STS_0   (WORD_TAG) 0
#define TAG_STS_1   (WORD_TAG) 1
#define TAG_STS_2   (WORD_TAG) 2
#define TAG_VIDEO   (WORD_TAG) 3
#define TAG_CONTROL (WORD_TAG) 4
#define TAG_SAY_HELLO_TO_DRONE  (WORD_TAG) 5
#define TAG_SAY_HELLO_TO_CC     (WORD_TAG) 6
#define TAG_STS_OK   (WORD_TAG) 7
#define TAG_TIMEOUT   (WORD_TAG) 8
#define TAG_KEY_EXCHANGE_DRONE   (WORD_TAG) 9
#define TAG_KEY_EXCHANGE_CC  (WORD_TAG) 10
#define TAG_UNDEFINED  (WORD_TAG) 11
#define TAG_STS_OK (WORD_TAG) 12
#define TAG_COMMAND 13

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

typedef struct lift_pkt_hdr{
	uint16_t tag;
	uint16_t len;
	uint16_t id;
} lift_pkt_hdr;

typedef struct lift_cmd_pkt{
	uint32_t seq_num;
	uint16_t cmd_type;
	uint16_t cmd;
} lift_cmd_pkt;





void getTLV(uint8_t* buf, uint16_t* buf_len, WORD_TAG tag,  WORD_LEN dataLength, WORD_ID id, uint8_t* data);
LIFT_RESULT decomposeTLV( WORD_TAG* tag,  WORD_LEN* dataLength,  uint32_t *crc,WORD_ID* id, uint8_t* data, uint8_t* buf, uint16_t buf_len);
void getTLVTest();

#endif
