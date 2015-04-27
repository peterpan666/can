#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

#include "lib.h"
#include "emission.h"

void USART3_Start(void);


//permet d'executer le process "timer_cligno" avec un temps de pause
uint8_t GLB_timer_emission = 0;

void emission_timer(void) {
	//Gestion de la variable temporelle de la tache cligno
	if(GLB_timer_emission != 0){
		GLB_timer_emission--;
	}
}

void emission_task(void) {
	//Machine d'etat de la tache cligno
	if(!GLB_timer_emission){
		GLB_timer_emission = conv_bdt(1000);//Base de temps de repetition de la tache cligno
		//GPIOD->ODR ^= GPIO_Pin_12;//Bagottage de la LED verte

		//printf("%4d\n",speed);

	}
}

void emission_start(void) {
	//Machine d'etat de la tache cligno
	if(!GLB_timer_emission){
		GLB_timer_emission = conv_bdt(1000);//Base de temps de repetition de la tache cligno

	}
}


// USART for debug Terminal
// ------------------------

void USART3_Start()
{
GPIO_InitTypeDef 		GPIO_InitStructure;
USART_InitTypeDef 		USART_InitStructure;

// Start GPIOC Clock for USART3
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

// Start APB1 clock for USART3
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

// GPIOC Configuration for USART3 (TX on PD8)
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
GPIO_Init(GPIOD, &GPIO_InitStructure);

// Connect the TX pin (PD8) to USART3 alternative function
GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

// Setup the properties of USART3
USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Tx;
USART_Init(USART3, &USART_InitStructure);

// Enable USART3
USART_Cmd(USART3, ENABLE);
}
