#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "yield.h"

struct ctx_s * current_ctx[CORE_NCORE];
struct ctx_s * ctx_ring[CORE_NCORE];
unsigned ctx_load[CORE_NCORE];

static unsigned last_pid = 0;

static void * initial_ebp;
static void * initial_esp;

void init_ctx_tab() {
	int i;
	for(i = 0; i < CORE_NCORE; i++) {
		current_ctx[i] = NULL;
		ctx_ring[i] = NULL;
		ctx_load[i] = 0;
	}
}

int init_ctx(struct ctx_s * ctx, size_t stack_size, funct_t f, void * arg,
		unsigned int core_id) {
	ctx->ctx_stack = malloc(stack_size);

	if (ctx->ctx_stack == NULL )
		return -1;

	ctx->ctx_ebp = ctx->ctx_esp = ((char*) ctx->ctx_stack)
			+ stack_size- STACK_WIDTH;

	ctx->ctx_f = f;
	ctx->ctx_arg = arg;
	ctx->ctx_state = CTX_INIT;
	ctx->ctx_magic = CTX_MAGIC;
	ctx->ctx_core_id = core_id;

    ctx->ctx_id = last_pid++;

	printf("Current ctx put on core #%d\n", ctx->ctx_core_id);

	return EXIT_SUCCESS;
}

int get_available_core(){

	unsigned i = 1, min = 1000, coreid=1;
	for(i = 1; i < CORE_NCORE; i++){
		unsigned load = ctx_load[i];
		if(min > load){
			min = load;
			coreid = i;
		}
	}

	return coreid;
}

void switch_to_ctx(struct ctx_s * ctx, unsigned core_id) {

	assert(ctx->ctx_magic == CTX_MAGIC);
	_out(TIMER_ALARM, 0xffffffff - TIMER_MSEC);

	while (ctx->ctx_state == CTX_END || ctx->ctx_state == CTX_STOP) {

		if (ctx_ring[core_id] == ctx) {
			ctx_ring[core_id] = current_ctx[core_id];
		}

		if (ctx == current_ctx[core_id]) {
			fprintf(stderr,
			 "All context in the ring are blocked for the core %d.\n",
			 core_id);

		}

		ctx = ctx->ctx_next;

		if (ctx->ctx_state == CTX_END) {
			free(ctx->ctx_stack);
			free(ctx);
			ctx_load[core_id]--;
		}

	}

	if (current_ctx[core_id] != NULL ) {
		void * esp = current_ctx[core_id]->ctx_esp;
		void * ebp = current_ctx[core_id]->ctx_ebp;
		asm ("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
				: "=r"(esp), "=r"(ebp)
				:);

	}
	current_ctx[core_id] = ctx;

	asm ("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
			:
			: "r"(current_ctx[core_id]->ctx_esp), "r"(current_ctx[core_id]->ctx_ebp));

	core_id = (unsigned) _in(CORE_ID);

	if (current_ctx[core_id]->ctx_state == CTX_INIT) {
		start_current_ctx();
	}

	irq_enable();

	return;

}

void start_current_ctx() {
	unsigned core_id = (unsigned) _in(CORE_ID);
	printf("start : %d\n", core_id);

	printf("Start the current context on the core #%d\n", core_id);
	irq_disable();

	current_ctx[core_id]->ctx_state = CTX_EXE;
	current_ctx[core_id]->ctx_f(current_ctx[core_id]->ctx_arg);
	current_ctx[core_id]->ctx_state = CTX_END;

	if (current_ctx[core_id]->ctx_next == current_ctx[core_id]) {
		asm ("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
				:
				: "r"(initial_esp), "r"(initial_ebp));
	}

}

int create_ctx(int stack_size, funct_t f, void* args) {
	irq_disable();


	struct ctx_s * new_ctx;

	//int core_id = get_available_core();
	int core_id = 1;


	ctx_load[core_id]++;



	new_ctx = malloc(sizeof(struct ctx_s));
	if (new_ctx == 0) {
		return -1;
	}

	if (ctx_ring[core_id] == NULL ) {
		ctx_ring[core_id] = new_ctx;
		new_ctx->ctx_next = new_ctx;
	} else {
		new_ctx->ctx_next = ctx_ring[core_id]->ctx_next;
		ctx_ring[core_id]->ctx_next = new_ctx;
	}


	init_ctx(new_ctx, (size_t) stack_size, f, args, core_id);
	printf("coreid : %d   core add : %p\n",new_ctx->ctx_core_id,ctx_ring[core_id]);
	irq_enable();

	return 0;
}

void yield() {
	unsigned core_id = (unsigned) _in(CORE_ID);
	_out(TIMER_ALARM, 0xffffffff - TIMER_MSEC);

	if (ctx_ring[core_id] == NULL ) {
		return;
	}

	if (current_ctx[core_id] != NULL ) {
		switch_to_ctx(current_ctx[core_id]->ctx_next, core_id);
	} else {
		asm ("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
				: "=r"(initial_esp), "=r"(initial_ebp)
				:);
		core_id = (unsigned) _in(CORE_ID);
		switch_to_ctx(ctx_ring[core_id], core_id);
	}

}
