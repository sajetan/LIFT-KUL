/*
 * random.h
 *
 *  Created on: Mar 17, 2020
 *      Author: Ferdinand Hannequart
 * 
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include"main.h"

#define SINGLEITEMS 4
#define SIZER 50 // should be large enough for personalization string, hash values and counter
#define SIZEPOOL    (SINGLEITEMS + 4*SIZER +1) 

typedef struct EntropyPool EntropyPool;
struct EntropyPool
{
    /* entropy inputs */
    WORD time;
    WORD bladeSpeed;
    WORD battery;
    WORD position;
    WORD image[SIZER];
    
    /* personalization string */
    WORD str[SIZER];
    
    /* counter */
    WORD counter[SIZER];

    /* half of previous hash */
    WORD previousHash[SIZER];

    /* gathers all values above in 1 array*/
    WORD complete[SIZEPOOL];
};


// functions for filling the Entropy pool
WORD getTime();
WORD getBladeSpeed();
WORD getBatteryLevel();
WORD getPosition();
void getImage(WORD image[]);
char * getString();

// Entropy pool functions
void initPool(EntropyPool* pool);
void accumulate(EntropyPool* pool, uint16_t n);
void updatePool(EntropyPool* pool);
void mergePool( EntropyPool* pool);
void hashPool(WORD w[], EntropyPool* pool);
void printPool(EntropyPool* pool);
void demoEntropyPool();


// other
void random(WORD rand[], uint32_t bit, EntropyPool* pool);
void randomTest();
void insertArray(WORD out[], WORD in[], WORD* start);

#endif /* RANDOM_H_ */
