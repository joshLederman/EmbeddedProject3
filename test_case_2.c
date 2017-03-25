#include "3140_concur.h"
#include "utils.h"

//Test to see if the program fails if one process terminates immediately

void p1 (void) {}

void p2 (void)
{
	int i;
	for (i=0; i < 10; i++) {
		delay ();
    LEDBlue_Toggle();
	}
}

int main (void)
{
 LED_Initialize();
 
 if (process_create (p1,20) < 0) {
 	return -1;
 }

 if (process_create (p2,20) < 0) {
 	return -1;
 }
 process_start ();
 
 LEDGreen_On();
 
 while (1) ;

 return 0;
}
