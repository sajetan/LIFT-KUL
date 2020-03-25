/*
Ferdinand Hannequart
main.c

purpose: running the tests
*/

#include"main.h"

void main(){

    WORD a[SIZE] = {0};
    char p[]= "10202992191299292273736782983983873766292309238287371723929392388299190902901277462346";
    hex_decoder(p,strlen(p), a);
	  print_num(a, a[0]);
  
    gcdTest();
    begcdTest();
    shiftl1Test(0);     // 0 is summary, 1 is everything
    shiftr1Test(0);     // 0 is summary, 1 is everything
    inverseTest();

}