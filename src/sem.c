#include "sem.h"

void sem_init(struct sem_s * sem, unsigned int val) {
	sem->sem_cpt = val;
	sem->sem_ctx = NULL;
}

void sem_down(struct sem_s * sem) {
	unsigned core_id = _in(CORE_ID);
	irq_disable();
	sem->sem_cpt--;
	if (sem->sem_cpt < 0) {
		current_ctx[core_id]->ctx_state = CTX_STOP;
		current_ctx[core_id]->ctx_sem_next = sem->sem_ctx;
		sem->sem_ctx = current_ctx[core_id];
		irq_enable();
		yield();
	} else {
		irq_enable();
	}

}

void sem_up(struct sem_s * sem) {
	irq_disable();
	sem->sem_cpt++;
	if (sem->sem_cpt <= 0) {
		sem->sem_ctx->ctx_state = CTX_EXE;
		sem->sem_ctx = sem->sem_ctx->ctx_sem_next;

	}
	irq_enable();

}

