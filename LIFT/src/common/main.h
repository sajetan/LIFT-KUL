/*
 * main.h
 * LIFT DRONE CONTROL PROJECT
 * Copyright: ESAT, KU Leuven
 * Author: Ferdinand Hannequart, Lien Wouters, Tejas Narayana
 * Year: 2020
 */

#ifndef MAIN_H_
#define MAIN_H_

#include"globals.h"

// Lien's functions
#include"add.h"
#include"sub.h"
#include"mod.h"
#include"mult.h"
#include"division.h"
#include"../ecc/signature.h"

// Ferdinand's functions
#include"inverse.h"
#include"shift.h"
#include "../hash/sha3.h"
#include "../hash/hash.h"
#include "../random/random_gen.h"
#include "communication.h"
#include "message.h"


// Tejas's functions
#include "utilities.h"
#include "../ecc/p256.h"
//#include"chacha20_poly1305_interface.h"

//FSM and comm
#include"fsm.h"



#endif
