/*
 * random.h
 *
 *  Created on: Mar 17, 2020
 *      Author: Ferdinand Hannequart
 * 
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "../common/globals.h"
#include "../common/utilities.h"
#include "../hash/hash.h"


#define SINGLEITEMS 4       // number of non-array values
#define SIZECOUNTER 64/BIT   //this value indicates the number of cells 
                            //that will contribute to the entropy hash. 
                            //To this number, 1 must be added for the length field
#define SIZEHALFHASH (256/BIT/2)  // this array will contain the data of the entropy pool.
#define SIZEPOOL (SINGLEITEMS + SIZEHALFHASH +  SIZECOUNTER +1)  // this array will contain the data of the entropy pool.


typedef struct EntropyPool EntropyPool;
struct EntropyPool
{
    /* entropy inputs */
    WORD time;
    WORD bladeSpeed;
    WORD battery;
    WORD position;
    //WORD image[SIZER];
    
    /* personalization string */
    //WORD str[SIZER];
    
    /* counter */
    uint64_t counter;
    WORD counterArray[SIZECOUNTER+1];

    /* half of previous hash */
    WORD halfHash[SIZEHALFHASH];

    /* gathers all values above in 1 array*/
    WORD complete[SIZEPOOL];

    /* used to stop the thread it is in */
    WORD kill;
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
void accumulate(EntropyPool* pool, uint32_t n);
void updatePool(EntropyPool* pool);
void mergePool( EntropyPool* pool);
void hashPool(WORD w[], EntropyPool* pool);
void printPool(EntropyPool* pool);
void demoEntropyPool();
//void *realtime_update_pool( void *ptr );
//pthread_t createPool(EntropyPool* pool);
//void killPool(EntropyPool* pool, pthread_t* thread_id);

// other
void random(WORD rand[], uint32_t bit, EntropyPool* pool);
void randomTest();


#endif /* RANDOM_H_ */
