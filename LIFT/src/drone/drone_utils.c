#include "../common/globals.h"
#include "drone_includes.h"


void get_temperature(uint8_t* temperature){
	printf("Inside (%s)\n", __func__);
	srand(time(0));
	WORD i=0;
	uint32_t data=(uint32_t)(rand() & 99);
	for(i=0;i<sizeof(uint32_t);i++){
		temperature[i] =data>>(8*i);
	}
	printf("Temperature: %2d degrees \n",temperature[0]);
}

void get_battery(uint8_t* battery){
	printf("Inside (%s)\n", __func__);
	srand(time(0));
	WORD i=0;
	uint32_t data=(uint32_t)(rand() & 99);
	for(i=0;i<sizeof(uint32_t);i++){
		battery[i] =data>>(8*i);
	}
	printf("Battery: %2d %% \n",battery[0]);
}

void get_gps_coordinates(uint8_t* coordinates){
	printf("Inside (%s)\n", __func__);
	WORD i=0;
	srand(time(0));
	uint32_t longitude = (50 << 24) |(rand() & 999999);
	uint32_t latitude  = (37 << 24) |(rand() & 999999);

	for(i=0;i<sizeof(uint32_t);i++){
		coordinates[i] =longitude>>(8*i);
		coordinates[i+sizeof(uint32_t)]=latitude>>(8*i);
	}
	printf("\nLONGITUDE: %02d.%02d%02d%02d \n",coordinates[3],coordinates[2],coordinates[1],coordinates[0]);
	printf("LATITUDE: %02d.%02d%02d%02d \n",coordinates[7],coordinates[6],coordinates[5],coordinates[4]);

}

