/*
 * drone_inlcudes.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef INCLUDE_DRONE_INCLUDES_H_
#define INCLUDE_DRONE_INCLUDES_H_

void get_temperature(uint8_t* temperature);
void get_battery(uint8_t* battery);
void get_gps_coordinates(uint8_t* coordinates);

#endif /* INCLUDE_DRONE_INCLUDES_H_ */
