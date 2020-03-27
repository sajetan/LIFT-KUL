/*
 * random.h
 *
 *  Created on: Mar 17, 2020
 *      Author: Ferdinand Hannequart
 */



#include"random.h"

/*returns a 256 bit random number
input: WORD array of size SIZE*/
void random(WORD rand[]){
    time_t current_time;
    char* c_time_string;
    WORD inputHash[SIZE] = {0};

    /* Obtain current time. */
    current_time = time(NULL);
    /* Convert to local time format. */
    c_time_string = ctime(&current_time);
    convert(inputHash,c_time_string );

    rand[0] = 256/BIT;
    hash256(rand,inputHash );
}

void randomTest(){
    printf("Test - random number: ");
    WORD w[SIZE] = {0};
    random(w);
    print_num(w);
}