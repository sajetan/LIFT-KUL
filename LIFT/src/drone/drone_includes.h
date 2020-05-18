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

void get_temperature(uint8_t* temperature);
void get_battery(uint8_t* battery);
void get_gps_coordinates(uint8_t* coordinates);

#endif /* INCLUDE_DRONE_INCLUDES_H_ */
