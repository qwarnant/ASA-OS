
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <hardware.h>

#include "hw.h"


void irq_disable() {

	_mask(15);
}

void irq_enable() {

	_mask(0);
}

