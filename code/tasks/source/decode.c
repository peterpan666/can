#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "lib.h"
#include "decode.h"
#include "globals.h"

//permet d'executer le process "timer_decode" avec un temps de pause
uint8_t timer_decode = 0;

void decode_timer(void) {
	//Gestion de la variable temporelle de la tache decode
	if(timer_decode != 0){
		timer_decode--;
	}
}

void decode_task(void) {
	//Machine d'etat de la tache decode
	if(!timer_decode){
		timer_decode = conv_bdt(1000);//Base de temps de repetition de la tache decode
		GPIOD->ODR ^= GPIO_Pin_13;//Bagottage de la LED verte
	}
}

void decode_init(void) {
	GPIO_InitTypeDef  	GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
