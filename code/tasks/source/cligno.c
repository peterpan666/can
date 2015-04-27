#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "lib.h"
#include "cligno.h"

//permet d'executer le process "timer_cligno" avec un temps de pause
uint8_t timer_cligno = 0;

void cligno_timer(void) {
	//Gestion de la variable temporelle de la tache cligno
	if(timer_cligno != 0){
		timer_cligno--;
	}
}

void cligno_task(void) {
	//Machine d'etat de la tache cligno
	if(!timer_cligno){
		timer_cligno = conv_bdt(1000);//Base de temps de repetition de la tache cligno
		GPIOD->ODR ^= GPIO_Pin_12;//Bagottage de la LED verte
	}
}

void cligno_init(void) {
	GPIO_InitTypeDef  	GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
