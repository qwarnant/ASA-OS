#ifndef HW_H
#define HW_H


typedef void (irq_handler_func_t)(void);

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ	2


void irq_disable();
void irq_enable();



#endif
