

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
void start_core();
int swap_ctx(unsigned core_src, unsigned core_dest);
int balance_ctx();
void manage_core();
void empty_it();
void doIt();