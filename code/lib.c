#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

#include "lib.h"

void USART3_Start(void)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    USART_InitTypeDef        USART_InitStructure;

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

void GPIO_Start(void)
{

	GPIO_InitTypeDef  	GPIO_InitStructure;

	// Start GPIOD Clock for LEDS
	// Start GPIOA Clock for Push-Button
	// Start GPIOE Clock for BNC debug signals

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Initialize LEDS IOs (GPIOD[12,13,14,15])

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// Initialize Push Button & Sign bit (GPIOA[0,1])

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Initialize BNC outputs (GPIOE[11,12,13,14])

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

}

void TIM5_Start(void)
{
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
