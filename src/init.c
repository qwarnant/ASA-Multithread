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
#include "hardware.h"
#include "init.h"
#include "irq.h"

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
    IRQVECTOR[TIMER_IRQ] = start_timer_core;
    _out(TIMER_PARAM,128+64); /* reset + alarm on */
    _out(TIMER_ALARM, 0xffffffff - 200);

	for(i = 0; i < CORE_NCORE-1; i++) {
		_out(CORE_IRQMAPPER + i, 0);
	}
	_out(CORE_UNLOCK, 0);

	_out(CORE_STATUS, 0xF);
	_out(CORE_IRQMAPPER + 1, 1 << TIMER_IRQ);

}

static void doIt() {
    int pow = 2;
    int i = 1;
    for (i = 1; i < 1000000000; i++) {
        pow *= 2;
    }
}

void empty_it() {

}


/**
* Exercice 1
*/
void start_core() {
    unsigned coreId = (unsigned) _in(CORE_ID);

    printf("Start the core #%d\n", coreId);

	while (1) {
        doIt();
		printf("End the job core #%d\n", coreId);
	}

}

/**
* Exercice 3
*/
void start_core_semaphore() {
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
void start_timer_core() {
	unsigned coreId = (unsigned) _in(CORE_ID);
	printf("Received timer IRQ from %d\n", coreId);
	 _out(TIMER_ALARM, 0xffffffff - 200);
}
