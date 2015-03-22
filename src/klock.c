#include <multicore.h>
#include <hardware.h>
#include <stdio.h>
#include "klock.h"
#include "irq.h"

void klock() {
    unsigned core_id = (unsigned) _in(CORE_ID);

    // Active waiting
    while(1) {

        unsigned locked = (unsigned) _in(CORE_LOCK);

        // If the lock is free, we take it and return

        if(locked == 0) {
            irq_disable();
            printf("Lock taken by the core #%d\n", core_id);
            _out(CORE_UNLOCK, 1);
            irq_enable();
            return;
        }
    }
}

void kunlock() {
    unsigned core_id = (unsigned) _in(CORE_ID);
    unsigned locked = (unsigned) _in(CORE_LOCK);

    if(locked == 0) {
        printf("The lock is not taken\n");
        return;
    }

    irq_disable();
    _out(CORE_UNLOCK, 0);
    printf("The lock has been released by the core #%d\n", core_id);
    irq_enable();
}