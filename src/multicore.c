/*
 * init.c
 *
 *  Created on: 9 févr. 2015
 *      Author: warnant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tools.h>
#include "hardware.h"
#include "multicore.h"
#include "irq.h"
#include "yield.h"
#include <readline/readline.h>
#include <readline/history.h>

static unsigned last_core_id = 0;

void init_multicore() {
	unsigned int i;
	char *c, HW_CONFIG[100];


	if ((c = getenv("HW_CONFIG")) == NULL )
		strcpy(HW_CONFIG, "core.ini");
	else
		strcpy(HW_CONFIG, c);

	if (init_hardware(HW_CONFIG) == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
	}

    printf("Initialize the core...\n");

    /* Interreupt handlers */
	for (i = 0; i < 16; i++)
		IRQVECTOR[i] = empty_it;

    /**
    * Exercice 2 - 3
    */
	IRQVECTOR[0] = start_core;
    IRQVECTOR[TIMER_IRQ] = manage_core;
    _out(TIMER_PARAM,128+64); /* reset + alarm on */
    _out(TIMER_ALARM, 0xffffffff - 20);

	for(i = 0; i < CORE_NCORE-1; i++) {
		_out(CORE_IRQMAPPER + i, 0);
	}
	_out(CORE_UNLOCK, 0);
    _out(CORE_STATUS, 0xF);

}

/**
* Exercice 1
*/
void start_core() {
    irq_enable();

    unsigned coreId = (unsigned) _in(CORE_ID);

    printf("Start the core #%d\n", coreId);

	_out(CORE_IRQMAPPER + coreId, 1 << TIMER_IRQ);


    while (1) {
        //doIt();
       // printf("End the job core #%d\n", coreId);
    }

}

/* Define the method called when a timer IRQ occurs */
void manage_core() {
    balance_ctx();
    yield();
}


void doIt() {
    int pow = 2;
    int i = 1;
    for (i = 1; i < 10000000; i++) {
        pow *= 2;
    }
}

void empty_it() {}



/**
* Exercice 3
*/
static void start_core_semaphore() {
    irq_enable();
    unsigned core_id = (unsigned) _in(CORE_ID);
	while(1) {

        unsigned locked = (unsigned) _in(CORE_LOCK);
        printf("Locked : %d\n", locked);
        if(locked == 0) {
            _out(CORE_UNLOCK, 1);
            printf("lock pris par le core #%d\n", core_id);
            doIt();
            _out(CORE_UNLOCK, 0);
        } else {
            printf("lock pas dispo pour le core #%d\n", core_id);
        }
    }
}

/**
* Exercice 2
 */
static void start_timer_core() {
	unsigned coreId = (unsigned) _in(CORE_ID);
	printf("Received timer IRQ from %d\n", coreId);
	 _out(TIMER_ALARM, 0xffffffff - 20);
}

/*
 * Define the method which manages the load on the cores
 */
int balance_ctx() {
    unsigned corei, corej, ctx_balanced_count = 0;

    for(corei = 0; corei < CORE_NCORE; corei++) {
        for(corej = corei + 1; corej < CORE_NCORE; corej++) {
            int diff = ctx_load[corei] - ctx_load[corej];
        	printf("Load average difference between the core : %d - %d with diff %d\n", corei, corej, diff);

            if(diff > 1) {
                if(swap_ctx(corei, corej) == RETURN_FAILURE) {
                    fprintf(stderr, "Error when balancing the load on the core : %d - %d with diff %d\n", corei, corej, diff);
                    continue;
                }
                ctx_balanced_count++;
            }
        }
    }

    return ctx_balanced_count;
}

/*
 * Define the method which swap the context load between
 * two cores
 */
int swap_ctx(unsigned core_src, unsigned core_dest) {
    struct ctx_s* temp, *prec = NULL, *targetCtx = NULL;

    irq_disable(); // => TODO replace with multicore semaphore to protect the list check core_semaphore method

    // Empty context ring
    if(ctx_ring[core_src] == NULL) {
        return RETURN_FAILURE;
    }

    temp = ctx_ring[core_src];
    prec = temp;


    // Searching for a context to swap
    // to the second core
    do {
        if(temp->ctx_state != CTX_END) {
            targetCtx = temp;
            break;
        }

        prec = temp;
        temp =  temp->ctx_next;

    } while(temp != ctx_ring[core_src]);

    // No swap configuration available
    if(targetCtx == NULL) {
        return RETURN_FAILURE;
    }

    // Change the main core
    prec->ctx_next = targetCtx->ctx_next;

    // Change the swap core
    if(ctx_ring[core_dest] != NULL) {
        temp = ctx_ring[core_dest];
        while(temp->ctx_next != ctx_ring[core_dest]) {
            temp = temp->ctx_next;
        }

        temp->ctx_next = targetCtx;
        targetCtx->ctx_next = ctx_ring[core_dest];

    }

    ctx_ring[core_dest] = targetCtx;

    // Update the load table
    ctx_load[core_dest]++;
    ctx_load[core_src]--;

    irq_enable(); // => TODO replace with multicore semaphore to protect the list check core_semaphore method

    return RETURN_SUCCESS;
}
