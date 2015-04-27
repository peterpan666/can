#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"

#include "lib.h"
#include "cligno.h"

int main(void){
	SystemInit();	//Initialisation du systeme

	//Initialisation des peripheriques
	GPIO_Start();
	USART3_Start();
	TIM5_Start();

    while(1){
    	cligno_task();
    }
}

//Interruption du Timer5 sur l'evenement "update" (base de temps)
void TIM5_IRQHandler(void)
{
	cligno_timer();

	//Bagottage de la led rouge
	GPIOD->ODR ^= GPIO_Pin_14;

	//On reset le flag d'interruption du Timer5
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
}

