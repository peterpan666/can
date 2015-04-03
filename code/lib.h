#ifndef _LIB_H_
#define _LIB_H_

//Constantes
//Période d'échantillonage en microseconde
#define Te_us 10000

//Macros

//retourne la valeur maximum entre a et b
#define MAX(a,b) (a>=b?a:b)
//retourne la valeur minimum entre a et b
#define MIN(a,b) (a<=b?a:b)
//Convertie la base de temps (ms) pour coller avec le Te_us choisi
#define conv_bdt(a) a*1000/Te_us

//défini un nouveau type de données "bool" qui peut prendre FALSE ou TRUE
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

void USART3_Start(void);
void GPIO_Start(void);
void TIM5_Start(void);

#endif
