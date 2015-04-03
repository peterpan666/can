#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#include "lib.h"
#include "cligno.h"

//permet d'ex�cuter le process "timer_cligno" avec un temps de pause
uint8_t GLB_timer_cligno = 0;

void cligno_task(void) {
	//Machine d'�tat de la tache cligno
	if(!GLB_timer_cligno){
		GLB_timer_cligno = conv_bdt(1000);//Base de temps de r�p�tition de la tache cligno
		GPIOD->ODR ^= GPIO_Pin_12;//Bagottage de la LED verte
	}
}
