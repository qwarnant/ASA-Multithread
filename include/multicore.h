#ifndef _MCORE_H_
#define _MCORE_H_

/*
* Define the number of active cores
*/
#define CORE_NCORE 4

/*
* Define which core is active
*/
#define CORE_STATUS 0x80

/*
* Define a vector for IRQ for each core
*/
#define CORE_IRQMAPPER 0x82

/*
* Define a registry which determines if the core lock is taken or not
* @see klock
*/
#define CORE_LOCK 0x98

/*
* Define a registry which manages the core lock value
* @see klock
*/
#define CORE_UNLOCK 0x99
#define CORE_ID 0x126
#define CORE_ENABLE 1

/*
* This method initiates all registries for the
* multi-core library and starts all core main functions.
*/
void init_multicore();

/*
* This method is the function started when a core is booting
*/
void start_core();

/**
* This method is called to swap a context between two cores
* with all their data
*/
int swap_ctx(unsigned core_src, unsigned core_dest);

/**
* This method is called to handle the size of context list
* of each core in order to manage the load of all cores
*/
int balance_ctx();

/**
* This method is used after each IRQ call to switch the current
* context on each core and balance the load
*/
void manage_core();

/**
* This method is a dummy function used in the IRQ vector
*/
void empty_it();

/*
* This method is a dummy function used in the main core
* function to do a loop dummy job
*/
void doIt();

#endif


