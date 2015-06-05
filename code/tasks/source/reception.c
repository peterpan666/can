#include "stm32f4xx.h"

#include "lib.h"
#include "globals.h"
#include "reception.h"

static uint8_t timer_reception = 0;
static uint8_t state_reception = 0;

void reception_timer(void) {
	if(timer_reception != 0){
		timer_reception--;
	}
}

void reception_task(void) {
	if(!timer_reception){
		switch (state_reception) {
			default:
				state_reception = 0;
			case 0:
				break;

		}
		timer_reception = conv_bdt(2000);
	}
}

void reception_init(void) {
}
