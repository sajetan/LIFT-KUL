/*
Ferdinand Hannequart
WORD.c
*/

#include"word.h"

/* prints a word[] in the following format:
"length x MSB ... LSB" 
e.g. for 16 bit: "2 x 178B 002F"*/
void printWord(WORD a[]){
    printf("%wd x ", a[0]);
    for(WORD i = a[0]; i>0; i--){
        printf("%p ", a[i]);
    }
    printf("\n");
}

/* copies array w[] to array copy[]
INPUT: 2 WORD[] arrays of length SIZE*/
void copyWord(WORD copy[], WORD w[]){
    for(WORD i = 0; i<=SIZE; i++){
        copy[i] = w[i];
    }
}

/*function for comparing two WORDS[].
BEWARE: the length field must be equal as well! 
e.g. {0} and {1, 0} are different 
INPUT: 2 WORD[] numbers
OUTPUT: 1 if equal, 0 if different*/
WORD equalWord(WORD a[], WORD b[]){
    WORD S1 = a[0];
    WORD S2 = b[0];

    if(S1 != S2){
        return 0;

    }
    else
    {
        for(WORD i = 1; i<=S1; i++){
            if(a[i] != b[i]){
                return 0;
            }
        }
        return 1;
    }
}

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
    if(pass){
        TESTOK()
    }
    else{
        TESTFAIL()
    }
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
    if(pass){
        TESTOK()
    }
    else{
        TESTFAIL()
    }
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

/*

void shiftr(WORD w[], WORD s){
    WORD smod  = s % BIT;
    WORD squot = s / BIT;
    WORD hasDisappeared = (w[w[0]] >> smod ) == 0 && w[0] != 0;

    for(WORD i=w[0]; i>squot; i--){
        w[i - squot] = (w[i] >> smod);
        if(smod != 0){ // to avoid shifting by a number equal to thel lenth of th word (undefined)
            w[i - squot] += w[i- 1] << (BIT-smod);
        }
    }
    w[0] = w[0] - squot - hasDisappeared;
}



void shiftl(WORD w[], WORD s){
    WORD squot = s / BIT;
    WORD smod  = s - s*squot;

    // shift each individual cell and append its righthand neighbour
    w[w[0] + squot +1] = w[0]  >> (BIT -1);
    for(WORD i=w[0]; i>0; i--){
        w[i + squot] = (w[i] << smod);
         The following for loop is necessary to avoid shifting by a number 
        equal to the bit length of that number (for example int a >> 32),
        since this operation is undefined. It also helps for shifting 
        the lsb value.
        if(smod != 0 && i != 1){ 
            w[i + squot] |= (w[i-1] >> (BIT-smod));
        }
        if(smod == 0 && i != 1){ 
            w[i + squot] |= w[i-1];
        }
    }
    // add zero values which appeared due to the shift
    for(WORD i=squot; i>0; i--){
        w[i] = 0;
    }
    // update length
    w[0] += squot +1; // number of possible new cells
    while(w[w[0]] == 0 && w[0]>0){
        w[0]--;
    }
}

void shiftlTest(){
    WORD t1[1]   = {0};
    WORD s1      = 0;
    WORD e1[1]   = {0};
    shiftlTestHelp(t1, s1, e1);
    
    WORD t2[2]   = {1, 1};
    WORD s2      = 0;
    WORD e2[2]   = {1, 1};
    shiftlTestHelp(t2, s2, e2);
    
    WORD t3[2]   = {1, 1};
    WORD s3      = 1;
    WORD e3[2]   = {1, 2};
    shiftlTestHelp(t3, s3, e3);

    WORD t4[2]   = {1, 1<<(BIT-1)};
    WORD s4      = 1;
    WORD e4[1]   = {0};
    shiftlTestHelp(t4, s4, e4);

    WORD t5[2]   = {1, 0};
    WORD s5      = 0;
    WORD e5[1]   = {0};
    shiftlTestHelp(t5, s5, e5);

    WORD t6[2]   = {1, 0};
    WORD s6      = 1;
    WORD e6[1]   = {0};
    shiftlTestHelp(t6, s6, e6);

    WORD t7[6]   = {5, 250, 123, 111, 0, 3};
    WORD s7      = 0;
    WORD e7[6]   = {5, 250, 123, 111, 0, 3};
    shiftlTestHelp(t7, s7, e7);

    WORD t8[6]   = {5, 250, 123, 111, 0, 3};
    WORD s8      = BIT;
    WORD e8[5]   = {4,0, 250, 123, 111};
    shiftlTestHelp(t8, s8, e8);


}


void shiftlTestHelp(WORD test[], WORD shift, WORD exp[]){
    printf("\n%s  shiftlTest() \n", __FILE__);
    printf("Test - ");
    printWord(test);
    printf("shift - %wd\n", shift);
    shiftl(test, shift);
    printf("Result - ");
    printWord(test);
    printf("Expect - ");
    printWord(exp);
}

void shiftlTestAuto(){
    const WORD STOP = 74;
    // tests with zero shifts
    WORD test0[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {1, 1}},
        {{1, 2},            {1, 2}},
        {{1, 1<<(BIT-2)},   {1,1<<(BIT-2) }},
        {{1, 1<<(BIT-1)},   {1,1<<(BIT-1) }},
        {{4, 0x89, 0x98, 0xFA, 0x09},   {4, 0x89, 0x98, 0xFA, 0x09}},
        {{4, ~0, ~0, ~0, ~0},   {4, ~0, ~0, ~0, ~0}},        
        {{STOP}, {STOP}},
    };
    // tests with 1 shift
    WORD test1[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {1, 2}},
        {{1, 2},            {1, 4}},
        {{1, 1<<(BIT-2)},   {1,1<<(BIT-1) }},
        {{1, 1<<(BIT-1)},   {2, 0, 1}},
        {{3, ~0, ~0, ~0},   {3, ~0 -1, ~0, ~0}},        
        {{3, ~0, ~0, 0},    {2, ~0 -1, ~0, 1}},
        {{3, 0, ~0, ~0},    {3, 0, ~0-1, ~0}},        
        {{3, 0, ~0, 0},     {3,0, ~0-1, 1}}, 
        {{4, 1,0, 0, 1<<(BIT-1)},   {5, 2, 0, 0, 0, 1}}, 
        {{4, 1<<(BIT-1),0, 0, 1},   {4, 0, 1, 0, 2}}, 
        {{7, 0, 0, 0, ( 1<<(BIT-1) )+1, 0, 0, 0},   {5, 0, 0, 0, 2, 1}},
        {{STOP}, {STOP}},
    };
    // tests with BIT shift
    WORD testBIT[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {2, 0, 1}},
        {{1, 2},            {2, 0, 2}},
        {{1, 1<<(BIT-2)},   {2, 0, 1<<(BIT-2) }},
        {{1, 1<<(BIT-1)},   {2, 0, 1<<(BIT-1) }},
        {{3, ~0, ~0, ~0},   {4, 0, ~0, ~0, ~0}},         
        {{5, 1, 0, 0, 0, 1<<(BIT-1)},   {2, 0, 1}},         
        {{7, 0, 0, 0, (1<<(BIT-1)) +1, 0, 0, 0},   {5, 0, 0, 0, 0,  (1<<(BIT-1)) +1}},
        {{STOP}, {STOP}},
    };
    // tests with 2*BIT+3 shifts
    WORD test2BITP3[100] [2] [100]  = {
        {{0},               {0}},
        {{1, 0},            {0}},
        {{1, 1},            {3, 0, 0, 1<<3}},
        {{1, 2},            {3, 0, 0, 16}},
        {{1, 1<<(BIT-2)},   {4, 0, 0, 0, 2 }},
        {{1, 1<<(BIT-1)},   {4, 0, 0, 0, 4 }},
        {{3, ~0, ~0, ~0},   {6, 0, 0, ~0 - 7, ~0, ~0, 7}},         
        {{5, 1, 0, 0, 0, 1<<(BIT-1)},   {8, 0, 0, 8, 0, 0, 0, 0, 4}},
        {{STOP}, {STOP}},
    };

    printf("\n\n \t ///////// Begin test %s ////// \n\n", __FILE__);

    for(WORD i = 0; test0[i][0][0] != STOP;i++){
        shiftlTestHelp(test0[i][0], 0, test0[i][1]);}

    for(WORD i = 0; test1[i][0][0] != STOP;i++){
        shiftlTestHelp(test1[i][0], 1, test1[i][1]);}

    for(WORD i = 0; testBIT[i][0][0] != STOP;i++){
        shiftlTestHelp(testBIT[i][0], BIT, testBIT[i][1]);}

    for(WORD i = 0; test2BITP3[i][0][0] != STOP;i++){
        shiftlTestHelp(test2BITP3[i][0], 2*BIT + 3, test2BITP3[i][1]);}
        
}
*/