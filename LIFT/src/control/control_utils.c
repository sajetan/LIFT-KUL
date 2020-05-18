/*
 * control_utils.c - contains basic control center utilities
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#include "control_includes.h"

int start_menu(){
	int choice=0;
	printf("\n-----------------------------------------------------------------\n");
	printf("Press (1) to establish Drone Connection\n");
	printf("Press (9) to Shutdown System\n");
	printf("Please enter your choice here: ");
	fflush(stdin);
	(void)scanf(" %d",&choice);
	(void)scanf("%*[^\n]");
	fflush(stdin);
	return choice;
}

int control_menu(){
	int choice=0;
	printf("\n-----------------------------------------------------------------\n");
	printf("Press (2) to Get Drone Status\n");
	printf("Press (3) to Send Command to Drone\n");
	printf("Press (4) to Start Stream Video from Drone\n");
	printf("Press (5) to Terminate Stream Video from Drone\n");
	printf("Press (6) to RESTART Session\n");
	printf("Press (9) to Shutdown System\n");
	printf("Please enter your choice here: ");
	(void)scanf(" %d",&choice);
	(void)scanf("%*[^\n]");
	return choice;
}

void commands_menu(){
	printf("-----------------------------------------------------------------\n");
	printf("\nBelow are the control options:\n");
	printf("Press (1) to turn the Drone RIGHT\n");
	printf("Press (2) to turn the Drone LEFT\n");
	printf("Press (3) to higher altitude of Drone by 1m\n");
	printf("Press (4) to lower altitude of Drone by 1m\n");
	printf("Please enter your choice here: ");
}

void status_menu(){
	printf("-----------------------------------------------------------------\n");
	printf("\nBelow are the status request options:\n");
	printf("Press (1) to get GPS coordinates of Drone\n");
	printf("Press (2) to get Battery level of Drone\n");
	printf("Press (3) to get Temperature value\n");
	printf("Please enter your choice here: ");
}
