#ifndef _Y_H_
#define _Y_H_

#include <stddef.h>
#include "hw.h"

/*	The magic value to verify the consistance of the context structure  */
#define CTX_MAGIC 0xCAFEBABE
#define STACK_WIDTH (16384*4)

typedef void funct_t(void *);

/* Define the current context structure */
extern struct ctx_s * current_ctx;
/* Define the context ring for the scheduler */
extern struct ctx_s * ctx_ring;

/* The available states of a context */
enum ctx_state_e {
	CTX_INIT, CTX_EXE, CTX_END, CTX_STOP
};

/* Define the context structure */
struct ctx_s {
	unsigned int ctx_id;
    unsigned int ctx_start_time;
    unsigned int ctx_exec_time;
	void * ctx_stack;
	void * ctx_esp;
	void * ctx_ebp;
	void * ctx_arg;
	funct_t * ctx_f;
	unsigned ctx_magic;
	struct ctx_s * ctx_next;
	enum ctx_state_e ctx_state;
	struct ctx_s * ctx_sem_next;
	struct ctx_s * ctx_hda_next;
};

/*
	create_ctx()
	This method creates a new context and put it on the context
	ring for the scheduler.
	The context will be initialized with a function and some args
 */
int create_ctx(int, funct_t, void*);

/*
	yield()
	This method allows the developer to suspend the current context and
	search the next unblocked context in the ring in order to execute it.
 */
void yield();
void yield_hw();
/*
	init_ctx()
	This method initializes a new context with a specified execution stack
	size, a function and some args.
 */
int init_ctx(struct ctx_s * ctx, size_t stack_size, funct_t f, void * arg);

/*
	switch_to_ctx()
	This method allows the developer to switch to an another
	context in the context ring.
 */
void switch_to_ctx(struct ctx_s * ctx);

/*
	start_current_ctx()
	This method starts the execution of the current context
 */
void start_current_ctx();

void get_state_name(enum ctx_state_e state, char* string);

#endif
