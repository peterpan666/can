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

#define POLYNOMIAL 0x1021
#define WIDTH  (8 * 16)
#define TOPBIT (1 << (WIDTH - 1))

typedef struct decd_frame {
	union {
		uint8_t tab[24];
		struct fields_11 {
			unsigned 	sof:1;
			unsigned 	id:11;
			unsigned	rtr:1;
			unsigned 	r0:1;
			unsigned 	r1:1;
			unsigned 	dlc:4;
			unsigned 	crc:16;
			unsigned	ack:2;
			unsigned 	eof:7;
			unsigned 	inter:3;
		} fields_11;
	} fixed_field;
	uint8_t data[8];
	uint8_t size;
	uint8_t wr_ind;
	uint8_t wr_off;
	unsigned bs_error:1;
	unsigned char bs_error_pos;
	unsigned crc_error:1;
	unsigned r0_error:1;
	unsigned dlc_error:1;
	unsigned id_error:1;
} decd_frame_t;

typedef struct frame {
	uint8_t size;
	uint8_t wr_ind;
	uint8_t wr_off;
	uint8_t buffer[24];
}frame_t;

//defini un nouveau type de donnees pour representer le buffer de reception (max 256 trames)
typedef struct recv_buffer {
	uint8_t read;
	uint8_t write;
	frame_t buffer[256];
} recv_buffer_t;

typedef struct decd_buffer {
	uint8_t read;
	uint8_t write;
	decd_frame_t buffer[256];
} decd_buffer_t;

typedef struct pT_frame {
	uint8_t ind;
	uint8_t off;
} pT_frame_t;

void write_bit_in_frame(decd_frame_t * frame, uint8_t bit_val);
uint8_t read_bit_in_frame(decd_frame_t* frame, pT_frame_t p);
void write_tab(uint8_t* tab, pT_frame_t p, uint8_t bit_val);
uint8_t read_tab(uint8_t * tab, pT_frame_t p);
uint32_t read_n_bit(uint8_t * tab, uint8_t spos, uint8_t nbits);
void get_pT_from_bit_pos(uint8_t pos, pT_frame_t * p);

// Calcul du crc
uint16_t crcTable[256];
void crcInit(void);
uint16_t crcFast(uint8_t const message[], int nBytes);


#endif
