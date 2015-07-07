#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "misc.h"

#include "lib.h"
#include "emission.h"

//déclarations des fonctions
static void USART3_Start(void);
static void GPIO_config(void);

extern decd_buffer_t GLB_decd_buffer;

//déclarations des variables
static uint8_t timer_emission = 0;
static bool flag_hyp_wait_cmd;
static char received_char = 0;
#define nb_message_accueil 7
static const char message_accueil[nb_message_accueil][70] = {
	 {"Sniffer BUS CAN\r\n"},
	 {"Presentation des commandes\r\n"},
	 {"Commande f = Affiche les trames decodees en continue\r\n"},
	 {"Commande d = Affiche les datas en continue\r\n"},
	 {"Commande b = Afficher la trame sans Bitstuffing\r\n"},
	 {"Commande s = Stopper l'affichage\r\n"},
	 {"Commande h = Menu d'aide\r\n\n"}
 };

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
	static uint8_t i = 0;
	static uint8_t machine_state = 0;
	int8_t j = 0;
	static decd_frame_t local_frame;

	if (flag_hyp_wait_cmd == TRUE && (machine_state == 0 || received_char == 's'))
	{
		machine_state = received_char;
		flag_hyp_wait_cmd=FALSE;
		timer_emission = 0;
	}

	if(!timer_emission){
		GPIOD->ODR ^= GPIO_Pin_15;//Bagottage de la LED bleue

		switch (machine_state) {
			case 's':
				i = 0;
				printf("\r\n");
			default:
				machine_state = 0;
			case 0:
				break;
			case 'h':
				if (i < nb_message_accueil) {
					printf("%s",message_accueil[i++]);
					timer_emission = conv_bdt(10);
				} else {
					i = 0;
					machine_state = 0;
				}
				break;
			case 'd':
				switch (i) {
					case 0:
						printf("Data en mode continu\r\n");
						i++;
						break;
					default:
						if (GLB_decd_buffer.read != GLB_decd_buffer.write) {
							printf("0x");
							local_frame = GLB_decd_buffer.buffer[GLB_decd_buffer.read++];
							for (j = (local_frame.fixed_field.fields_11.dlc - 1); j >= 0; j--) {
								printf("%x",local_frame.data[j]);
							}
							printf("\r\n");
						}
						break;
				}

				timer_emission = conv_bdt(10);//Base de temps de repetition de la tache cligno
				break;
			case 'f':
				switch (i++) {
					case 0:
						printf("|SOF|  ID   |RTR|R0 |R1 |DLC|        DATA        |  CRC   | ACK | EOF  |INTER|\r\n");
						printf("==============================================================================\r\n");
						break;
					case 1:
						if (GLB_decd_buffer.read != GLB_decd_buffer.write) {
							local_frame = GLB_decd_buffer.buffer[GLB_decd_buffer.read++];
							printf("| ");
						} else {
							i = 1;
						}
						break;
					case 2:
						printf("%d", local_frame.fixed_field.fields_11.sof);
						printf(" | ");
						break;
					case 3:
						printf("0x%x", local_frame.fixed_field.fields_11.id);
						printf(" | ");
						break;
					case 4:
						printf("%d", local_frame.fixed_field.fields_11.rtr);
						printf(" | ");
						break;
					case 5:
						printf("%d", local_frame.fixed_field.fields_11.r0);
						printf(" | ");
						break;
					case 6:
						printf("%d", local_frame.fixed_field.fields_11.r1);
						printf(" | ");
						break;
					case 7:
						printf("%d", local_frame.fixed_field.fields_11.dlc);
						printf(" | ");
						break;
					case 8:
						printf("%s","0x");
						for (j = 7; j >= 0; j--) {
							if (j < local_frame.fixed_field.fields_11.dlc) {
								printf("%2x",local_frame.data[j]);
							} else {
								printf("00");
							}

						}
						printf(" | ");
						break;
					case 9:
						printf("0x%x", local_frame.fixed_field.fields_11.crc);
						printf(" | ");
						break;
					case 10:
						printf("0x%x", local_frame.fixed_field.fields_11.ack);
						printf(" | ");
						break;
					case 11:
						printf("0x%x", local_frame.fixed_field.fields_11.eof);
						printf(" | ");
						break;
					case 12:
						printf("0x%x", local_frame.fixed_field.fields_11.inter);
						break;
					default:
						printf(" |\r\n");
						i = 1;
						break;
				}
				timer_emission = conv_bdt(10);//Base de temps de repetition de la tache cligno
				break;
		}
	}
}


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

	// GPIOC Configuration for USART3 (TX on PD8 yellow & RX on PD9 orange)
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


void USART3_IRQHandler(void){
	// check if the USART3 receive interrupt flag was set
	if( USART_GetITStatus(USART3, USART_IT_RXNE) ){
		received_char = USART3->DR;
		flag_hyp_wait_cmd = TRUE;
		USART_ClearFlag(USART3, USART_IT_RXNE);
	}
}
