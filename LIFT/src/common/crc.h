/*
 * crc.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdio.h>
#include <stdint.h>
#include"globals.h"

void generate_crc_tabble();
uint32_t compute_crc(uint8_t *data, uint16_t len);


#endif /* CRC_H_ */
