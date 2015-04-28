#include "stm32f4xx.h"

#include "lib.h"
#include "reception.h"

static uint8_t timer_reception = 0;

void reception_timer(void) {
	if(timer_reception != 0){
		timer_reception--;
	}
}

void reception_task(void) {
	if(!timer_reception){
		timer_reception = conv_bdt(2000);
	}
}

void reception_init(void) {
}
