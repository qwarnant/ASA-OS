
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <hardware.h>
#include <stdio.h>

#include "hw.h"


void irq_disable() {
   // printf("irq disable\n");

	_mask(15);
}

void irq_enable() {
   // printf("irq enable\n");
	_mask(0);
}

