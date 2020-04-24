/*
 * sig_gen.h

 *
 *  Created on: Mar 25, 2020
 *      Author: r0665956
 */

#ifndef SIG_GEN_H_
#define SIG_GEN_H_

#include"signature.h"

void sig_gen_pre(WORD *r, WORD *k, WORD *n, WORD *G_x, WORD *G_y, WORD size);
void sig_gen(WORD *s, WORD *m, WORD *k, WORD *n, WORD *d, WORD WORD_length, WORD size);


#endif /* SIG_GEN_H_ */
