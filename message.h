/*
 * message.h
 * 
 *
 *  Created on: April 9, 2020
 *      Author: Ferdinand Hannequart
 */

#ifndef DEF_MESSAGE
#define DEF_MESSAGE

#include<time.h>        // for timing
#include<sys/time.h>    // for timing
#include<stdio.h>       // for prints
#include<stdint.h>      // for uintx_t
#include<pthread.h>     // threads
#include<unistd.h>      // for sleep()
//#include <stdlib.h>
#include<stddef.h>
#include<string.h>
#include <assert.h>


#define WORD uint16_t

#define SIZEHASH 256

#define NUMBEROFBITS 258    // max number of bits our arrays will have to hold
#define WORD_2 uint32_t
#define WORD_3 uint64_t
#define BYTE sizeof(WORD)
#define BIT (BYTE*8)
#define SIZE ((NUMBEROFBITS- (NUMBEROFBITS % BIT))/BIT +1 + ((NUMBEROFBITS % BIT) != 0))
//#define SIZE 19


/*
    Format of STS message:
    type    :   [   Tag         Length      ID          ||  Data    ]
    bytes   :   [   BYTE_TAG    BYTE_LEN    BYTE_ID     ||  ?       ]
    bytes   :   [   1           2           8           ||  ?       ]
*/

#define WORD_TAG    uint8_t
#define WORD_LEN    uint16_t
#define WORD_ID     uint64_t

#define BYTE_TAG    sizeof(WORD_TAG)
#define BYTE_LEN    sizeof(WORD_LEN)
#define BYTE_ID     sizeof(WORD_ID)
#define BYTE_DATA_MAX   10             // maximum number of bytes of data

#define BYTE_MESSAGE_MAX (BYTE_TAG + BYTE_LEN + BYTE_ID + BYTE_DATA_MAX)

#define TAG_STS_0   (WORD_TAG) 0
#define TAG_STS_1   (WORD_TAG) 1
#define TAG_STS_2   (WORD_TAG) 2
#define TAG_VIDEO   (WORD_TAG) 3
#define TAG_CONTROL (WORD_TAG) 4
#define TAG_SAY_HELLO_TO_DRONE  (WORD_TAG) 5
#define TAG_SAY_HELLO_TO_CC     (WORD_TAG) 6

#define MAX_NUMBER_SESSIONS

typedef struct Session Session;
struct Session
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

void getTLV(uint8_t* message, int* message_length, WORD_TAG tag,  WORD_LEN dataLength, WORD_ID id, uint8_t* data);
void getTLVTest();

#endif