#ifndef _LIB_H_
#define _LIB_H_

//Constantes
//Periode d'echantillonage en microseconde
#define Te_us 10000

//Macros

//retourne la valeur maximum entre a et b
#define MAX(a,b) (a>=b?a:b)
//retourne la valeur minimum entre a et b
#define MIN(a,b) (a<=b?a:b)
//Convertie la base de temps (ms) pour coller avec le Te_us choisi
#define conv_bdt(a) a*1000/Te_us

//defini un nouveau type de donnees "bool" qui peut prendre FALSE ou TRUE
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

//defini un nouveau type de donnees pour representer les trames (maximum 24 octets)
typedef struct recv_frame {
	uint8_t size;
	uint8_t data[24];
} recv_frame_t;

//defini un nouveau type de donnees pour representer le buffer de reception (max 256 trames)
typedef struct recv_buffer {
	uint8_t read;
	uint8_t write;
	recv_frame_t buffer[256];
} recv_buffer_t;


#endif
