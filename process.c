#include "3140_concur.h"
#include <stdlib.h>
#include <fsl_device_registers.h>


struct process_state {
			unsigned int *sp;
			unsigned int *sp_original; //Original stack pointer
			unsigned int size; //Size of stack as initialized
			struct process_state *nextProcess;
}; 

struct process_state * current_process = NULL;
//The queue will be created with process_create in begin_queue
//then switched to current_process during the first call to process_select
struct process_state * begin_queue = NULL;

void append(struct process_state * lastElement, struct process_state * list){
			struct process_state *tmp;
			//current_process - list of process_state
			if (list == NULL) {
				list = lastElement;
				lastElement->nextProcess = NULL;
			}
			else {
				tmp = list;
				while (tmp->nextProcess != NULL) {
					// while there are more elements in the list
					tmp = tmp->nextProcess;
				}
				// now tmp is the last element in the list
				tmp->nextProcess = lastElement;
				lastElement->nextProcess = NULL;
			}
}

//Removes first process of queue
struct process_state* remove() {
	//Creating new pointer to process at top of list
	struct process_state * removed_process = current_process;
	//Moving next process up, effectively removing top process
	current_process = removed_process->nextProcess;
	//Returning the pointer to the top process;
	return removed_process;
}

int process_create (void (*f)(void), int n) {
			unsigned int *sp = process_stack_init(*f, n);
			if (sp == NULL) return -1;
			struct process_state *processState = malloc(sizeof(*processState));
			processState->sp = sp;
			processState->sp_original = sp;
			processState->size=n;
			append(processState, begin_queue);
			return 0;
};

void process_start (void) {
	
	NVIC_EnableIRQ(PIT0_IRQn); //Enables interrupts
	
	PIT_MCR = 1; //Enables standard timers
	
	process_begin();
}
	
unsigned int * process_select (unsigned int *cursp) {
	if (cursp==NULL) {
		if (current_process==NULL) {
			//This is the first call to process_select
			current_process=begin_queue;
			begin_queue=NULL; //To ensure once all programs have terminated that current_process becomes NULL
			return current_process->sp;
		}
		else {
			//A process has just terminated
			
			//Remove the process from the queue
			struct process_state * terminated_process = remove();
			//Free the stack of the process
			process_stack_free(terminated_process->sp_original, terminated_process->size);
			//Free the struct holding the process
			free(terminated_process);
			//Return the next process
			return current_process->sp;
		}
	}
	else {
		//Switching from one running process to another
		
		//Store the next sp
		current_process->sp=cursp;
		//Remove the top process from the queue and add to end
		struct process_state * switched_process = remove();
		append(switched_process, current_process);
		//Returns the new process sp
		return current_process->sp;
	}
}
