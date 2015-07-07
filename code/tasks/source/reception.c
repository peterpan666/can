#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "misc.h"

#include "lib.h"
#include "reception.h"

//déclarations des fonctions
static void USART2_Start(void);
static void GPIO_config(void);

static uint8_t timer_reception = 0;
static const uint16_t timeout = conv_bdt(1000);
static uint8_t machine_state = 0;
static uint8_t i = 0;

extern recv_buffer_t GLB_recv_buffer;

void reception_timer(void) {
	if(timer_reception != 0){
		timer_reception--;
	}
}

void reception_task(void) {
	if(!timer_reception){
		machine_state = 0;
	}

	if (machine_state == 0) {
		GPIOD->ODR &= ~GPIO_Pin_13;
	}
}

void reception_init(void) {
	USART2_Start();
	GPIO_config();
}

void USART2_Start()
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	USART_InitTypeDef 		USART_InitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	// Start GPIOA Clock for USART2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Start APB1 clock for USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// GPIOA Configuration for USART2 (TX on PA2 yellow & RX on PA3 orange)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Connect the TX pin (PA2) to USART2 alternative function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	// Connect the RX pin (PA3) to USART2 alternative function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// Setup the properties of USART2
	//ATTENTION AU HSE dans le fichier stm32f4xx.h (8MHz) quartz externe
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);

	//Setup the USART2 interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	// we want to configure the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART3 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	// this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART2 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);


	// Enable USART2
	USART_Cmd(USART2, ENABLE);

}


void GPIO_config()
{
	GPIO_InitTypeDef  	GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//Initialize GPIO[15] output pin on PORT D
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void USART2_IRQHandler(void){
	// check if the USART2 receive interrupt flag was set
	if( USART_GetITStatus(USART2, USART_IT_RXNE) ){
		//printf("%2x\r\n", USART2->DR);
		switch(machine_state) {
			case 0:
				if (USART2->DR == 0x04) {
					timer_reception = timeout;
					i = 0;
					machine_state++;
					GPIOD->ODR |= GPIO_Pin_13;
				}
				break;
			case 1:
				GLB_recv_buffer.buffer[GLB_recv_buffer.write].size = USART2->DR;
				machine_state++;
				//printf("%2x", GLB_recv_buffer.buffer[GLB_recv_buffer.write].size);
				timer_reception = timeout;
				break;
			case 2:
				GLB_recv_buffer.buffer[GLB_recv_buffer.write].buffer[i++] = USART2->DR;

				if (i == GLB_recv_buffer.buffer[GLB_recv_buffer.write].size) {
					GLB_recv_buffer.write++;
					machine_state++;
				} else {
					timer_reception = timeout;
				}
				break;
			default:
				machine_state = 0;
				break;
		}

		USART_ClearFlag(USART2, USART_IT_RXNE);
	}
}
