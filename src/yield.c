#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "yield.h"
#include "init.h"

struct ctx_s * current_ctx[CORE_NCORE];
struct ctx_s * ctx_ring[CORE_NCORE];

static unsigned last_core_id = 0;

static void * initial_ebp;
static void * initial_esp;

int init_ctx(struct ctx_s * ctx, size_t stack_size, funct_t f, void * arg) {
	struct ctx_s* temp;

	ctx->ctx_stack = malloc(stack_size);

	if (ctx->ctx_stack == NULL )
		return -1;

	ctx->ctx_ebp = ctx->ctx_esp = ((char*) ctx->ctx_stack)
			+ stack_size- STACK_WIDTH;

	ctx->ctx_f = f;
	ctx->ctx_arg = arg;
	ctx->ctx_state = CTX_INIT;
	ctx->ctx_magic = CTX_MAGIC;
	ctx->ctx_core_id = (last_core_id++) % CORE_NCORE;

	return EXIT_SUCCESS;
}

void switch_to_ctx(struct ctx_s * ctx) {

	assert(ctx->ctx_magic == CTX_MAGIC);
	irq_disable();

	unsigned core_id = _in(CORE_ID);

	while (ctx->ctx_state == CTX_END || ctx->ctx_state == CTX_STOP) {
		if (ctx_ring[core_id] == ctx) {
			ctx_ring[core_id] = current_ctx;
		}

		if (ctx == current_ctx) {
			fprintf(stderr,
					"All context in the ring are blocked for the core %d.\n",
					core_id);
		}

		ctx = ctx->ctx_next;

		if (ctx->ctx_state == CTX_END) {
			free(ctx->ctx_stack);
			free(ctx);
		}
	}

	if (current_ctx != NULL ) {

		asm ("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
				: "=r"(current_ctx->ctx_esp), "=r"(current_ctx->ctx_ebp)
				:);

	}
	current_ctx[core_id] = ctx;

	asm ("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
			:
			: "r"(current_ctx[core_id]->ctx_esp), "r"(current_ctx[core_id]->ctx_ebp));

	if (current_ctx[core_id]->ctx_state == CTX_INIT) {
		start_current_ctx();
	}
	irq_enable();
	return;

}

void start_current_ctx() {
	unsigned core_id = _in(CORE_ID);

	current_ctx[core_id]->ctx_state = CTX_EXE;
	current_ctx[core_id]->ctx_f(current_ctx->ctx_arg);
	current_ctx[core_id]->ctx_state = CTX_END;

	if (current_ctx[core_id]->ctx_next == current_ctx[core_id]) {
		asm ("movl %0, %%esp" "\n\t" "movl %1, %%ebp"
				:
				: "r"(initial_esp), "r"(initial_ebp));
	}

}

int create_ctx(int stack_size, funct_t f, void* args) {
	struct ctx_s * new_ctx;
	unsigned core_id = _in(CORE_ID);

	new_ctx = malloc(sizeof(struct ctx_s));
	if (new_ctx == 0) {
		return -1;
	}

	if (ctx_ring[core_id] == NULL ) {
		ctx_ring[core_id] = new_ctx;
		new_ctx->ctx_next = new_ctx;
	} else {
		new_ctx[core_id]->ctx_next = ctx_ring[core_id]->ctx_next;
		ctx_ring[core_id]->ctx_next = new_ctx;

	}
	init_ctx(new_ctx, (size_t) stack_size, f, args);

	return 0;
}

void yield() {
	unsigned core_id = _in(CORE_ID);

	if (current_ctx[core_id] != NULL ) {
		switch_to_ctx(current_ctx[core_id]->ctx_next);
	} else {
		asm ("movl %%esp, %0" "\n\t" "movl %%ebp, %1"
				: "=r"(initial_esp), "=r"(initial_ebp)
				:);
		switch_to_ctx(ctx_ring[core_id]);
	}

}
