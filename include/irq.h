#ifndef IRQ_H
#define IRQ_H


typedef void (irq_handler_func_t)(void);

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8

/*
* Define the number of the timer IRQ in the IRQ vector
*/
#define TIMER_IRQ	2

/*
* Define the timer time between each IRQ call
*/
#define TIMER_MSEC  2000

/*
 * This method is used to disable all IRQ signal in a code execution
*/
void irq_disable();

/*
 * This method is used to enable all IRQ signal in a code execution
*/
void irq_enable();


#endif
