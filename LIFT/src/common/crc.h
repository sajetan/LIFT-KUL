/*
 * crc.h
 *
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdio.h>
#include <stdint.h>

void generate_crc_tabble();
uint32_t compute_crc(uint8_t *data, uint16_t len);


#endif /* CRC_H_ */