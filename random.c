/*
 * random.h
 *
 *  Created on: Mar 17, 2020
 *      Author: Ferdinand Hannequart
 */



#include"random.h"

/*returns a 256 bit random number
input: WORD array of size SIZE*/
void random(WORD rand[], uint32_t bit, EntropyPool* pool){
    WORD intermediate[SIZE] = {0};
    uint32_t i = 0;
    uint32_t start = 0;
    uint32_t max = (128/BIT);

    // get values
    start = 1;
    while(bit>((start-1)*BIT)){
        hashPool(intermediate, pool);
        for(i = 0; i<max ;i++ ){
            rand[start + i] = intermediate[i+1];
        }
        start +=i;
    }

    // update length
    start--;
    while(rand[start] == 0 && start>0){
        start--;
    }
    rand[0] = start;
}

/*  returns the current time in microsecond. If word = uint8_t, 
    there is overflow since the returned value an be up to 999 999,
    but we don't care since only the lower lsb have sufficient randomness*/
WORD getTime(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return   (WORD) (tv.tv_usec) ;
}

/* returns the rotor blade speed */
WORD getBladeSpeed(){
    // code 
    return 0;
}

/* returns the battery level */
WORD getBatteryLevel(){
    // code 
    return 0;
}

/* returns the GPS coordinates*/
WORD getPosition(){
    // code 
    return 0;
}

/* returns image from camera*/
void getImage(WORD image[]){
    WORD i = 0;
    for (i = 1; i<SIZER; i++){
        image[i] = 0;
    }
    i--;
    while(image[i] == 0 && i>0){
        i--;
    }
    image[0] = i;
}

/* returns the personalization string*/
char *getString(){
    return "abc";
}

/*  This function initializes all variables of the pool to zero.
    Only exception: the personalization string is already inserted,
    since it should be added once only.
*/
void initPool(EntropyPool* pool){
    WORD i = 0;
    char *s = getString();

    /* entropy inputs */
    pool->time = 0;
    pool->bladeSpeed = 0;
    pool->battery = 0;
    pool->position = 0;

    /*  image + personalization string + counter + half of previous hash */
    for(i = 0; i<SIZER; i++){
        pool->image[i] = 0;
        pool->str[i] = 0;
        pool->counter[i] = 0;
        pool->previousHash[i] = 0;
    }
    getString(s);
    text2array(pool->str, s); // convert string s to array str in the pool


    /* complete  */
    for(i = 0; i<SIZEPOOL; i++){
        pool->complete[i] = 0;

    }
}

/* 
    Update the entropy pool.
    Whenever this function is called, each item of the entropy pool is updated
    and the counter is incremented by one.
*/
void updatePool(EntropyPool* pool){
    pool->time              = getTime();
    pool->bladeSpeed        = getBladeSpeed();
    pool->battery           = getBatteryLevel();
    pool->position          = getPosition();
    getImage(pool->image);

    mergePool( pool);  
}

/*
    Merge all items of the entropy pool into the "pool.complete" array.
    If a single item is zero, the associated array element is zero.
    For the array items, only the relevant bits are added (all zero MSB are left out).
    Moreover, these arrays are added sequentially directly behind eachother.
 */
void mergePool( EntropyPool* pool){
    WORD start = 0;

    // first add the single items
    start= 1;
    pool->complete[start+0] = pool->time;
    pool->complete[start+1] = pool->bladeSpeed;
    pool->complete[start+2] = pool->battery;
    pool->complete[start+3] = pool->position;

    // then add the arrays
    start += SINGLEITEMS;
    insertArray(pool->complete, pool->image,        &start);
    insertArray(pool->complete, pool->str,          &start);
    insertArray(pool->complete, pool->counter,      &start);
    insertArray(pool->complete, pool->previousHash, &start);
    
    // and finally determine the new size.
    start--;
    while(pool->complete[start] == 0 && start>0){
        start--;
    }
    pool->complete[0] = start;
}

/*
    Compute the hash of the entropy pool.
    More specifically, the "pool.complete" array is hashed.
    The lower bits are passed on, the upper bits return in the pool.
*/
void hashPool(WORD w[], EntropyPool* pool){
    WORD i      = 0;
    WORD half   = 256/(BIT*2);

    mergePool(pool);                // merge all items into array pool.complete
    hash256(w, pool->complete);     // compute the hash of pool.compete

    // update the half hash value of the pool
    for(i =0; i<half; i++){
        pool->previousHash[i+1] = w[i +1 +half ];
    }
    pool->previousHash[0] = half;

    // pass on only the lower bits by updating the length field
    w[0] = half;

    add1(pool->counter);   // increase counter by 1

}

/*
    Iteratively updates and computes the hash of the entropy pool
    to build up entropy. This function is used in the very beginning, when 
    the entropy pool has been created but lacks sufficient entropy.
*/
void accumulate(EntropyPool* pool, uint16_t n){
    WORD i = 0;
    WORD w[SIZE] = {0};

    for(i = 0; i<n; i++){
        updatePool(pool);
        hashPool(w, pool);
    }
}

/*
    Auxiliary function to easily insert small arrays in bigger ones
*/
void insertArray(WORD out[], WORD in[], WORD* start){
    WORD i = 1;
    for(i = 1; i<=in[0]; i++){
        out[*start +i-1] = in[i];
    }
    *start +=i-1;
}

/* 
    Displays the elements of the entropy pool in a graphically pleasing way
*/
void printPool(EntropyPool* pool){
    printf("----------- Entropy pool ----------\n\n");
    printf("\t time            = %x\n", pool->time);
    printf("\t rotorBladeSpeed = %x\n", pool->bladeSpeed);
    printf("\t battery         = %x\n", pool->battery);
    printf("\t GPS             = %x\n", pool->position);
    printf("\t image           = "); print_num(pool->image);

    printf("\n");

    printf("\t personalization string = "); print_num(pool->str);

    printf("\n");

    printf("\t counter          = "); print_num(pool->counter);

    printf("\n");

    printf("\t previous hash     = "); print_num(pool->previousHash);
    printf("\n");

    printf("\t complete       = "); print_num(pool->complete);
    printf("-------------------------------\n\n");

}

void demoEntropyPool(){
	EntropyPool pool;

	initPool(&pool);

    printf("\n////////////////////// begin demo entropy pool ////////////////////////////////////////\n\n");
    printf("// \t First the entropy pool is initialized:\n");
	printPool(&pool);

    printf("// \t Let's update its values once:\n");
	updatePool(&pool);
	printPool(&pool);

    printf("// and a second time.\n"
            "// counter should still be at 0, as well as the previous hash\n"
            "// all the other values can be found back in the array 'complete'\n");
	updatePool(&pool);
	printPool(&pool);

    printf("// Now we will build up the entropy by successively updating\n"
            " and hashing the pool. \n"
            " the field previousHash receives now the upper part of the previous hash\n"
            " Let's do it 100 times\n"
            " counter should now be at 100 (64):\n");

	accumulate(&pool, 100);
	printPool(&pool);

    printf("// Now the entropy pool is ready for usage!\n");
        printf("\n////////////////////////////////////////\n\n");

}


void randomTest(){
    EntropyPool pool;
    WORD a[SIZE] = {0};
    uint32_t n = 10;
    
    BEGINTEST(1)
	
    initPool(&pool);
    
    n = 0;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 0 bits:"); print_num(a);
    
    n = 1;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 1 bits:"); print_num(a);
    
    n = 10;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 10 bits:"); print_num(a);
       
    n = 128;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 128 bits:"); print_num(a);
    
    n = 129;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 129 bits:"); print_num(a);
    

    n = 256;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 256 bits:"); print_num(a);
    
    n = 257;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 257 bits:"); print_num(a);

    ENDTEST(1)

}