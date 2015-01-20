#ifndef _S_H_
#define _S_H_

#include "yield.h"

/*
	Define the semaphore structure
 */
struct sem_s {
	int sem_cpt;
	struct ctx_s * sem_ctx;
};

/*
	sem_init()
	This method initiates a new semaphore structure to a target value
 */
void sem_init(struct sem_s *, unsigned int);

/*
	sem_down()
	This method decreases the value of a specified semaphore
	If the value is negative, the current context is blocked
 */
void sem_down(struct sem_s *);

/*
	sem_up()
	This method increases the value of a specified semaphore
	If the value is lower or equal to zero, the first blocked context
	is released
 */
void sem_up(struct sem_s *);

#endif
