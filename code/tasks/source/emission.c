#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "misc.h"

#include "lib.h"
#include "emission.h"
#include "globals.h"

//memset(received_string,0,MAX_LENGTH);//Flush: init tous les caracters a vide
//déclarations des fonctions
void USART3_Start(void);
void GPIO_config(void);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);

//void main_send_RS232(void);


//déclarations des variables
uint8_t timer_emission = 0;
uint16_t usart3_tmp=0;
bool flag_hyp_wait_cmd;
uint8_t i = 0,j=0,k=0;



//static const uint8_t MAX_LENGTH=7;
#define MAX_LENGTH 7
volatile char received_string[MAX_LENGTH + 1];


void emission_timer(void) {
	//Gestion de la variable temporelle de la tache cligno
	if(timer_emission != 0){
		timer_emission--;
	}
}

void emission_init(void)
{
	GPIO_config();
	USART3_Start();
	flag_hyp_wait_cmd=FALSE;
}

void emission_task(void) {
	//Machine d'etat de la tache cligno
	if(!timer_emission){
		timer_emission = conv_bdt(1000);//Base de temps de repetition de la tache cligno
		GPIOD->ODR ^= GPIO_Pin_15;//Bagottage de la LED bleue

		for (   j=0; j<5;j++)
		{
			printf("%s",GLB_message_accueil[j]);
		}



		/*if (flag_hyp_wait_cmd==TRUE)
		{
			printf("%s",received_string);
			flag_hyp_wait_cmd=FALSE;
			memset(received_string,0,MAX_LENGTH);//Flush: init tous les caracters a vide

		}*/
	}
}



/*
void main_send_RS232(void)
{
    if(GLB_send_finish == 0)
    {
        if(GLB_message[GLB_current_character] != '\0')
        {
               GLB_data_to_send = GLB_message[GLB_current_character];
               printf("%c",GLB_data_to_send);
               GLB_current_character++;
         }
        else
        {
            GLB_current_character = 0;
            GLB_send_finish = 1;
        }
    }
}


*/




// USART3 (Rx & Tx) for debug Terminal
// ----------------------------------s
void USART3_Start()
{
GPIO_InitTypeDef 		GPIO_InitStructure;
USART_InitTypeDef 		USART_InitStructure;
NVIC_InitTypeDef		NVIC_InitStructure;

// Start GPIOD Clock for USART3
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

// Start APB1 clock for USART3
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

// GPIOC Configuration for USART3 (TX on PD8 & RX on PD9)
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
GPIO_Init(GPIOD, &GPIO_InitStructure);

// Connect the TX pin (PD8) to USART3 alternative function
GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
// Connect the RX pin (PD9) to USART3 alternative function
GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

// Setup the properties of USART3
//ATTENTION AU HSE dans le fichier stm32f4xx.h (8MHz) quartz externe
USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
USART_Init(USART3, &USART_InitStructure);

//Setup the USART3 interrupt
USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt

NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	// we want to configure the USART3 interrupts
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART3 interrupts
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	// this sets the subpriority inside the group
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	// the USART3 interrupts are globally enabled
NVIC_Init(&NVIC_InitStructure);


// Enable USART3
USART_Cmd(USART3, ENABLE);

}

void GPIO_config()
{
	GPIO_InitTypeDef  	GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//Initialize GPIO[15] output pin on PORT D
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


//Interruption sur UART3
void USART3_IRQHandler(void){
	// check if the USART3 receive interrupt flag was set
	if( USART_GetITStatus(USART3, USART_IT_RXNE) ){

		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART3->DR; // the character from the USART3 data register is saved in t

		/* check if the received character is not the LF character (used to determine end of string)
		 * or the if the maximum string length has been reached
		 */
		if( (t != 33) && (cnt < MAX_LENGTH) ){ //33=!
			received_string[cnt] = t;
			cnt++;
		}
		else{ // otherwise reset the character counter and print the received string
			cnt = 0;
			//USART_puts(USART3, received_string);
			flag_hyp_wait_cmd = TRUE; //Commande reçu

		}
	}
}
/*
void USART_puts(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*s++;
	}
}*/
