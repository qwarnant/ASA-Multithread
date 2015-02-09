/*
 * init.c
 *
 *  Created on: 9 févr. 2015
 *      Author: warnant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "init.h"

void init_multicore() {
	unsigned int i;
	char *c, HW_CONFIG[100];

	printf("val : %s\n", HW_CONFIG);

	if ((c = getenv("HW_CONFIG")) == NULL )
		strcpy(HW_CONFIG, "core.ini");
	else
		strcpy(HW_CONFIG, c);

	printf("%s\n", HW_CONFIG);

	if (init_hardware(HW_CONFIG) == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
	}

	/* Interreupt handlers */
	for (i = 0; i < 16; i++)
		IRQVECTOR[i] = empty_it;

	IRQVECTOR[0] = start_core;
	IRQVECTOR[TIMER_IRQ] = start_timer_core;

	_out(TIMER_ALARM, 0xFFFFFFFF - 20); /* alarm at next tick (at 0xFFFFFFFF) */
	_out(TIMER_PARAM, 128 + 64); /* reset + alarm on */

	printf("Initialize the core...\n");

	for(i = 0; i < CORE_NCORE-1; i++) {
		_out(CORE_IRQMAPPER + i, 0);
	}

	_out(CORE_IRQMAPPER + 1, 1 << TIMER_IRQ);



	_out(CORE_STATUS, 0xF);

}

void empty_it() {

}

void start_core() {
	printf("Start the core\n");
	unsigned coreId = _in(CORE_ID);
	int pow = 2;

	while (1) {
		int i = 1;
		for (i = 1; i < 65000; i++) {
			pow *= 2;
		}
		printf("End the job core #%d\n", coreId);
	}

}

void start_timer_core() {
	unsigned coreId = _in(CORE_ID);
	printf("Received timer IRQ from %d\n", coreId);
	_out(TIMER_ALARM, 0xFFFFFFFF - 20);
}
