#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"

#include "lib.h"
#include "cligno.h"
#include "decode.h"
#include "emission.h"

void TIM5_init(void);

int main(void) {
	//Initialisation du systeme
	SystemInit();

	//Initialisation des peripheriques
	TIM5_init();

	cligno_init();
	decode_init();

    while(1){
    	cligno_task();
    	decode_task();
    }
}

//Interruption du Timer5 sur l'evenement "update" (base de temps)
void TIM5_IRQHandler(void) {
	cligno_timer();
	decode_timer();

	//Bagottage de la led rouge
	GPIOD->ODR ^= GPIO_Pin_14;

	//On reset le flag d'interruption du Timer5
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
}

void TIM5_init(void) {
	GPIO_InitTypeDef  	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Active l'horloge sur le Timer5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInitTypeDef Timer_Init_5ms;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable the TIM5 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseStructInit(&Timer_Init_5ms);
	Timer_Init_5ms.TIM_Prescaler = 168>>1;	//Prescaler de 168/2=84
	Timer_Init_5ms.TIM_Period = Te_us;		//Le timer enverra une interruption update lorsqu'il arrivera à la valeur Te_us
	Timer_Init_5ms.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5,&Timer_Init_5ms);
	TIM_Cmd(TIM5, ENABLE);

	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
}

