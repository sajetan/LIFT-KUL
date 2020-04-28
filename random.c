/*
 * random.h
 *
 *  Created on: Mar 17, 2020
 *      Author: Ferdinand Hannequart
 */



#include"random.h"

/* 
    Returns a random number of specified length.
    INPUTS:     -pool   :   structure entropy pool
                -bit    :   32 bit number specifying the desired number of random bits.
                            obviously, it mut be smaller than the maximum allowed number of bits given the type and length of the output rray
    OUTPUTS:    -rand   :   array of WORD of size SIZE, contains a number that is "bit" bits long (or less since the MSB's are also random and thus can be zero)
*/
void random(WORD *rand, uint32_t bit, EntropyPool* pool){
    assert(bit<=NUMBEROFBITS);

    WORD intermediate[SIZE] = {0};  // will contain the 128 bit random output
    uint32_t i = 0;                 // will iterate over the 128 bit random output
    uint32_t start = 0;             // will iterate over the number of 128 bit numbers needed
    uint32_t max = (SIZEHASH/BIT/2);
    uint8_t shift = 0;

    // get values from the hashPool function and put them in the array
    start = 1;
    while(bit>((start-1)*BIT)){
        updatePool(pool);
        hashPool(intermediate, pool);
        for(i = 0; i<max && (i + start-1)*BIT < bit ;i++ ){
            rand[start + i] = intermediate[i+1];
        }
        start +=i;                                      
    }
    start--;
    
    // remove excess bits
    shift = (bit % BIT == 0) ? 0 :  ( BIT - bit % BIT );

    rand[start] <<= shift;
    rand[start] >>= shift;




    // update length of the output array
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
    return ~0;
}

/* returns the battery level */
WORD getBatteryLevel(){
    // code 
    return ~0;
}

/* returns the GPS coordinates*/
WORD getPosition(){
    // code 
    return ~0;
}

/* returns image from camera*/
void getImage(WORD image[]){
    WORD i = 0;
    for (i = 1; i<SIZE; i++){
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
    //char *s = getString();

    /* entropy inputs */
    pool->time = 0;
    pool->bladeSpeed = 0;
    pool->battery = 0;
    pool->position = 0;

    /*  image + personalization string + counter + half of previous hash */
    /*for(i = 0; i<SIZER; i++){
        pool->image[i] = 0;
        pool->str[i] = 0;
        pool->counter[i] = 0;
        pool->previousHash[i] = 0;
    }
    text2array(pool->str, s); // convert string s to array str in the pool
    */


    /* complete  */
    //for(i = 0; i<SIZEPOOL; i++){
    //    pool->complete[i] = 0;
   // }

    /* half of the privious hash  */
    for(i = 0; i<SIZEHALFHASH; i++){
        pool->halfHash[i] = 0;
    }

    /* counter  */
    for(i = 0; i<(SIZECOUNTER+1); i++){
        pool->counterArray[i] = 0;
    }
    pool->counter = 0;

    /* complete  */
    for(i = 0; i<SIZEPOOL; i++){
        pool->complete[i] = 0;
    }



    /* kill signal for thread */
    pool->kill = 0;
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
    mergePool(pool);
}

/*
    Merge all items of the entropy pool into the "pool.complete" array.
    If a single item is zero, the associated array element is zero.
    For the array items, only the relevant bits are added (all zero MSB are left out).
    Moreover, these arrays are added sequentially directly behind eachother.
 */
void mergePool( EntropyPool* pool){
    WORD start = 0;
    WORD i = 0;


    // insert values into 'complete' array
    start= 1;
    pool->complete[start+0] = pool->time;
    pool->complete[start+1] = pool->bladeSpeed;
    pool->complete[start+2] = pool->battery;
    pool->complete[start+3] = pool->position;
    start += SINGLEITEMS; 


    // insert the counter
    for (i = 0; i<SIZECOUNTER; i++){
        pool->complete[start+i] = pool->counterArray[i+1] ; // counterArray[i+1] since length field not included
    }
    start += SIZECOUNTER; 


    // insert the upper half of the previous hash
    for (i = 0; i<SIZEHALFHASH; i++){
        pool->complete[start+i] = pool->halfHash[i] ; 
    }
    start += SIZEHALFHASH; 

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
    The lower 128 bits are passed on, the upper 128 bits return in the pool.
*/
void hashPool(WORD w[], EntropyPool* pool){
    WORD i      = 0;
    WORD half   = SIZEHASH/(BIT*2);

    mergePool(pool); // only important place is here

    hash(w, pool->complete, SIZEHASH);     // compute the hash of pool.complete
    // update the upper half hash value of the pool
    for(i =0; i<half; i++){
        pool->halfHash[i] = w[i +1 +half ];
        w[i +1 +half ] = 0;
    }

    // pass on only the lower bits by updating the length field
    w[0] = half;

    pool->counter++;   // increase counter by 1

number2array(pool->counterArray, pool->counter); // convert counter number to array
}


/*
    Iteratively updates and computes the hash of the entropy pool
    to build up entropy. This function is used in the very beginning, when 
    the entropy pool has been created but lacks sufficient entropy.
*/
void accumulate(EntropyPool* pool, uint32_t n){
    uint32_t i = 0;
    WORD w[SIZE] = {0};
    for(i = 0; i<n; i++){
        updatePool(pool);
        hashPool(w, pool);
    }
}
/*
void *realtime_update_pool( void *ptr ){
    EntropyPool* pool = (EntropyPool *) ptr;    // cast pool
    P(101)
    P(pool->kill)
    while(!pool->kill){
    //while(1){
        P(11)
        updatePool(pool);
        P(12)	
        P(77)
        printPool(pool);
    }
}

pthread_t createPool(EntropyPool* pool){
    // 1. initialize pool by setting all elements to 0
    initPool(pool);

    // 2. create a thread that updates the values of the pool in real time
    pthread_t thread_id;
	pthread_create(&thread_id, NULL, realtime_update_pool, &pool);
    
    // 3. build up entropy
    //accumulate(pool, 100);

    // 4. return thread identity, will be used to kill the thread
    return thread_id;
}

void killPool(EntropyPool* pool, pthread_t* thread_id){

    // Set kill value to 1, which will cause the while loop
    // of the thread to exit.
    pool->kill = 1;

    // Wait for the thread to finish
    pthread_join(*thread_id, NULL); 
}



void managePoolThread( void *ptr )
{    
    EntropyPool* pool = (EntropyPool*) ptr;    // cast pool

    initPool(pool);  // initialize pool

    accumulate(pool, 100);    // let the pool accumulate entropy


    while(1){
         update
     }
     pool = (char *) ptr;
     printf("%s \n", message);
}
*/



/* 
    Displays the elements of the entropy pool in a graphically pleasing way
*/
void printPool(EntropyPool* pool){
    printf("----------- Entropy pool ----------\n\n");
    printf("\t time            = %x\n", pool->time);
    printf("\t rotorBladeSpeed = %x\n", pool->bladeSpeed);
    printf("\t battery         = %x\n", pool->battery);
    printf("\t GPS             = %x\n", pool->position);
    /*printf("\t image           = "); print_num(pool->image);

    printf("\n");

    printf("\t personalization string = "); print_num(pool->str);*/

    printf("\n");

    printf("\t counter uint32_t             = %ld\n", pool->counter); 
    printf("\t counter [len, lsb->msb]      = ");  print_array(pool->counterArray, SIZECOUNTER+1);

    printf("\n");

    printf("\t half hash  [ lsb->msb]       = "); print_array(pool->halfHash, SIZEHALFHASH);
    printf("\n");

    printf("\t complete [len, lsb->msb]     = "); print_array(pool->complete, SIZEPOOL);
    printf("-------------------------------\n\n");

}

void demoEntropyPool(){
	EntropyPool pool;
    WORD w[SIZE] = {0};

	initPool(&pool);

    printf("\n////////////////////// begin demo entropy pool ////////////////////////////////////////\n\n");
    printf("// \t First the entropy pool is initialized:\n");
	printPool(&pool);

    printf("// \t Let's update its values once:\n");
	updatePool(&pool);
	printPool(&pool);

    printf("// and a second time.\n"
            "// counter should still be at 0, as well as the previous hash\n"
            "// and the complete array");
	updatePool(&pool);
	printPool(&pool);

    printf("// Let's merge the pool\n");
	mergePool(&pool);
	printPool(&pool);

    printf("// Now we hash it \n");
	hashPool(w,&pool);
	printPool(&pool);

    printf("// and merge again \n");
	mergePool(&pool);
	printPool(&pool);

    printf("// Now we will build up the entropy by successively updating,\n"
            " merging and hashing the pool. \n"
            " the field previousHash receives now the upper part of the previous hash\n"
            " Let's do it 100000 times\n"
            " counter should now be at around 100000 + previous :\n");

	accumulate(&pool, 1);
	printPool(&pool);

    printf("// Now the entropy pool is ready for usage!\n");
        printf("\n////////////////////////////////////////\n\n");

}


void randomTest(){
    EntropyPool pool;
    WORD a[SIZE] = {0};
    uint32_t n = 10;
    initPool(&pool);        // initialize pool
    accumulate(&pool, 100); // mix the pool 100 times

    BEGINTEST(1)

    for(int i = 0; i<10; i++){
            n = i;
            random(a, n, &pool);
            //printPool(&pool);
            printf("random number of n = %d bits:", n); print_num(a);
    }
	

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

    n = 256;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 257 bits:"); print_num(a);

    n = 256;
    random(a, n, &pool);
    //printPool(&pool);
    printf("random number of n = 258 bits:"); print_num(a);
    
    
    ENDTEST(1)

}
