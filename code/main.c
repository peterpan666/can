#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"

#include "lib.h"
#include "cligno.h"

//permet d'ex�cuter le process "timer_cligno" avec un temps de pause
extern uint8_t GLB_timer_cligno;

int main(void){
	SystemInit();	//Initialisation du syst�me

	//Initialisation des p�riph�riques
	GPIO_Start();
	USART3_Start();
	TIM5_Start();

    while(1){
    	cligno_task();
    }
}

//Interruption du Timer5 sur l'�v�nement "update" (base de temps)
void TIM5_IRQHandler(void)
{
	cligno_timer();

	//Bagottage de la led rouge
	GPIOD->ODR ^= GPIO_Pin_14;

	//On reset le flag d'interruption du Timer5
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
}

