#include "3140_concur.h"
#include <stdlib.h>
#include <fsl_device_registers.h>

process_t *current_process = NULL;

struct process_t {
	unsigned int * sp;
	struct process_state nextProcess;
}

void process_start (void) {
	
	NVIC_EnableIRG(PIT0_IRQn); //Enables interrupts
	
	PIT_MCR = 1; //Enables standard timers
	
	process_begin();
}
	
unsigned int * process_select (unsigned int *cursp) {
	