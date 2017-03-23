#include "3140_concur.h"
#include <stdlib.h>
#include <fsl_device_registers.h>

process_t * process_list = NULL;
process_t * current_process = NULL;

struct process_state {
			unsigned int *sp;
			struct process_state *nextProcess;
}; 

void append(struct process_state lastElement){
			struct process_state *tmp;
			//process_list - list of process_state
			if (process_list == NULL) {
				process_list = &lastElement;
				lastElement.nextProcess = NULL;
			}
			else {
				tmp = process_list;
				while (tmp->nextProcess != NULL) {
					// while there are more elements in the list
					tmp = tmp->nextProcess;
				}
				// now tmp is the last element in the list
				tmp->nextProcess = &lastElement;
				lastElement.nextProcess = NULL;
			}
}

int process_create (void (*f)(void), int n) {
			unsigned int *sp = process_stack_init(*f, n);
			if (sp == NULL) return -1;
			struct process_state *processState = malloc(sizeof(*processState));
			processState->sp = sp;
			append(*processState);
			return 0;
};



void process_start (void) {
	
	NVIC_EnableIRG(PIT0_IRQn); //Enables interrupts
	
	PIT_MCR = 1; //Enables standard timers
	
	process_begin();
}
	
unsigned int * process_select (unsigned int *cursp) {
	if (current_process == NULL) {
		return NULL;
	}
	current_process = current_process->nextProcess;
	return current_process->sp;
}
