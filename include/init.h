

#define CORE_ENABLE 1
#define CORE_NCORE 4
#define CORE_STATUS 0x80
#define CORE_IRQMAPPER 0x82
#define CORE_ID 0x126
#define CORE_LOCK 0x98
#define CORE_UNLOCK 0x99

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ	2


void init_multicore();
void empty_it();
void start_core();
void start_timer_core();
void start_core_semaphore();
