/*
Ferdinand Hannequart
shift.c
*/

#include"shift.h"



/* Shift function: shifts a WORD[] number once to the right*/
void shiftr1(WORD w[]){
    // if w[0] == 0, nothing changes. Else:
    if(w[0] != 0){
        for(WORD i=1; i<w[0]; i++){
            w[i] = (w[i] >> 1) | (w[i+1] << (BIT-1));
        }
        w[w[0]] >>= 1; // we do not consider the neigbour for the last element
        // update length
        if(w[w[0]] == 0){
            while(w[w[0]] == 0 && w[0]>0){
                w[0]--;
            }
        }
    }
}


/* Shift function: shifts a WORD[] number once to the left*/
void shiftl1(WORD w[]){
    // compute upper cell
    WORD upperValue =  w[w[0]] >> (BIT -1);
    
    // compute middle cells
    for(WORD i=w[0]; i>1; i--){
        w[i] = (w[i] << 1) | (w[i-1] >> (BIT-1));
    }

    //compute lower cell
    w[1] = w[1] << 1;

    // update length
    if(upperValue == 0){
        while(w[w[0]] == 0 && w[0]>0){
            w[0]--;
        }
    }
    else{
        w[w[0]+1] = upperValue;
        w[0]++;
    }
}

/* Test function of shiftr1(), prints all the tests in a readable way */
void shiftr1Test(WORD print){
    const WORD STOP = 74;
    BEGINTEST(print)
    // tests with 1 shift
    WORD test1[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {0}},
        {{1, 2},            {1, 1}},
        {{1, 1<<(BIT-2)},   {1,1<<(BIT-3) }},
        {{1, 1<<(BIT-1)},   {1,1<<(BIT-2) }},
        {{3, ~0, ~0, ~0},   {3, ~0, ~0, ~0-(1<<(BIT-1))}},        
        {{3, ~0, ~0, 0},    {2, ~0, ~0-(1<<(BIT-1))}},
        {{3, 0, ~0, ~0},    {3, 1<<(BIT-1), ~0, ~0-(1<<(BIT-1))}},        
        {{5, 0, ~0, ~0, 0, 0},{3, 1<<(BIT-1), ~0, ~0-(1<<(BIT-1))}},        
        {{4, 0,  0, ~0, ~0},{4, 0, 1<<(BIT-1), ~0, ~0-(1<<(BIT-1))}},        
        {{3, 0, ~0, 0},     {2,(1<<(BIT-1)), ~0-(1<<(BIT-1))}}, 
        {{4, 1,0, 0, 1<<(BIT-1)},   {4, 0, 0, 0, 1<<(BIT-2)}}, 
        {{4, 1<<(BIT-1),0, 0, 1},   {3, 1<<(BIT-2), 0, 1<<(BIT-1)}}, 
        {{7, 0, 0, 0, ( 1<<(BIT-1) )+1, 0, 0, 0},   {4, 0, 0, 1<<(BIT-1), 1<<(BIT-2)}},
        {{STOP}, {STOP}},
    };
    
    WORD pass = 1;
    for(WORD i = 0; test1[i][0][0] != STOP && pass;i++){
        pass &= shiftr1TestHelp(test1[i][0], test1[i][1], print);
    }
    TEST(pass)
    ENDTEST(print)
}

/* This subfunction is used by shiftr1Test()
and contains all the print instructions*/
WORD shiftr1TestHelp(WORD test[], WORD exp[], WORD print){
    WORD c[SIZE];
    copyWord(c, test);
    shiftr1(test);
    if(print){
        printf("\n%s  shiftl1Test() line \n", __FILE__);
        printf("Test - ");
        printWord(c);
        printf("Result - ");
        printWord(test);
        printf("Expect - ");
        printWord(exp);
    }
    return equalWord(test,exp);
}

/* Test function of shiftl1(), prints all the tests in a readable way */
void shiftl1Test(WORD print){
    const WORD STOP = 74;

    BEGINTEST(print)

    // tests with 1 shift
    WORD test1[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {1, 2}},
        {{1, 2},            {1, 4}},
        {{1, 1<<(BIT-2)},   {1,1<<(BIT-1) }},
        {{1, 1<<(BIT-1)},   {2, 0, 1}},
        {{3, ~0, ~0, ~0},   {4, ~0 -1, ~0, ~0, 1}},        
        {{3, ~0, ~0, 0},    {3, ~0 -1, ~0, 1}},
        {{3, 0, ~0, ~0},    {4, 0, ~0-1, ~0, 1}},        
        {{5, 0, ~0, ~0, 0, 0},{4, 0, ~0-1, ~0, 1}},        
        {{4, 0,  0, ~0, ~0},{5, 0, 0, ~0-1, ~0, 1}},        
        {{3, 0, ~0, 0},     {3,0, ~0-1, 1}}, 
        {{4, 1,0, 0, 1<<(BIT-1)},   {5, 2, 0, 0, 0, 1}}, 
        {{4, 1<<(BIT-1),0, 0, 1},   {4, 0, 1, 0, 2}}, 
        {{7, 0, 0, 0, ( 1<<(BIT-1) )+1, 0, 0, 0},   {5, 0, 0, 0, 2, 1}},
        {{STOP}, {STOP}},
    };

    WORD pass = 1;
    for(WORD i = 0; test1[i][0][0] != STOP && pass;i++){
        pass &= shiftl1TestHelp(test1[i][0], test1[i][1], print);
    }
    TEST(pass)
    ENDTEST(print)
}

/* This subfunction is used by shiftl1Test()
and contains all the print instructions*/
WORD shiftl1TestHelp(WORD test[], WORD exp[], WORD print){
    WORD originalTest[SIZE];
    copyWord( originalTest,test);
    shiftl1(test);
    if(print){
        printf("\n%s  shiftl1Test() line \n", __FILE__);
        printf("Test - ");
        printWord(originalTest);
        printf("Result - ");
        printWord(test);
        printf("Expect - ");
        printWord(exp);
    }
    return equalWord(test,exp);
}
