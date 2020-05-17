/*
 * drone_inlcudes.h
 */

#ifndef INCLUDE_DRONE_INCLUDES_H_
#define INCLUDE_DRONE_INCLUDES_H_

typedef enum {
	DRONE_SESSION_INIT = 0,
	DRONE_CALCULATE_SESSION_KEY,
	DRONE_VALIDATE_SESSION,
	DRONE_SESSION_COMMUNICATION,
	DRONE_SESSION_TERMINATE
} LIFT_DRONE_SESSION_STATES;

typedef struct {
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

} drone_memory;



#endif /* INCLUDE_DRONE_INCLUDES_H_ */
