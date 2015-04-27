#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#include "lib.h"
#include "decode.h"

//permet d'exécuter le process "timer_decode" avec un temps de pause
uint8_t GLB_timer_decode = 0;

void decode_timer(void) {
	//Gestion de la variable temporelle de la tache decode
	if(GLB_timer_decode != 0){
		GLB_timer_decode--;
	}
}

void decode_task(void) {
	//Machine d'état de la tache decode
	if(!GLB_timer_decode){
		GLB_timer_decode = conv_bdt(1000);//Base de temps de répétition de la tache decode
		GPIOD->ODR ^= GPIO_Pin_13;//Bagottage de la LED verte
	}
}
