#ifndef _Y_H_
#define _Y_H_

#include <stddef.h>
#include "irq.h"
#include "multicore.h"
#include "hardware.h"

/*	The magic value to verify the consistance of the context structure  */
#define CTX_MAGIC 0xCAFEBABE
#define STACK_WIDTH 16384

typedef void funct_t(void *);

/* Define the current context structure */
extern struct ctx_s * current_ctx[CORE_NCORE];
/* Define the context ring for the scheduler */
extern struct ctx_s * ctx_ring[CORE_NCORE];
/* Define the context load on the cores */
extern unsigned ctx_load[CORE_NCORE];

/* The available states of a context */
enum ctx_state_e {
	CTX_INIT, CTX_EXE, CTX_END, CTX_STOP
};

/* Define the context structure */
struct ctx_s {

    unsigned ctx_id;
	void * ctx_stack;
	void * ctx_esp;
	void * ctx_ebp;
	void * ctx_arg;
	funct_t * ctx_f;
	unsigned ctx_magic;
	struct ctx_s * ctx_next;
	enum ctx_state_e ctx_state;
	struct ctx_s * ctx_sem_next;

	/**
	 * Core id
	 */
	unsigned ctx_core_id;
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

/*
 init_ctx()
 This method initializes a new context with a specified execution stack
 size, a function and some args.
 */
int init_ctx(struct ctx_s * ctx, size_t stack_size, funct_t f, void * arg, unsigned int);

/*
 switch_to_ctx()
 This method allows the developer to switch to an another
 context in the context ring.
 */
void switch_to_ctx(struct ctx_s * ctx,unsigned);

/*
 start_current_ctx()
 This method starts the execution of the current context
 */
void start_current_ctx();

void init_ctx_tab();

int get_available_core();

#endif
