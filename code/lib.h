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
//typedef struct recv_frame {
//	uint8_t size;
//	union {
//		uint8_t data[24];
//		struct {
//			unsigned b0:1;
//			unsigned b1:1;
//			unsigned b2:1;
//			unsigned b3:1;
//			unsigned b4:1;
//			unsigned b5:1;
//			unsigned b6:1;
//			unsigned b7:1;
//			unsigned b8:1;
//			unsigned b9:1;
//			unsigned b10:1;
//			unsigned b11:1;
//			unsigned b12:1;
//			unsigned b13:1;
//			unsigned b14:1;
//			unsigned b15:1;
//			unsigned b16:1;
//			unsigned b17:1;
//			unsigned b18:1;
//			unsigned b19:1;
//			unsigned b20:1;
//			unsigned b21:1;
//			unsigned b22:1;
//			unsigned b23:1;
//			unsigned b24:1;
//			unsigned b25:1;
//			unsigned b26:1;
//			unsigned b27:1;
//			unsigned b28:1;
//			unsigned b29:1;
//			unsigned b30:1;
//			unsigned b31:1;
//			unsigned b32:1;
//			unsigned b33:1;
//			unsigned b34:1;
//			unsigned b35:1;
//			unsigned b36:1;
//			unsigned b37:1;
//			unsigned b38:1;
//			unsigned b39:1;
//			unsigned b40:1;
//			unsigned b41:1;
//			unsigned b42:1;
//			unsigned b43:1;
//			unsigned b44:1;
//			unsigned b45:1;
//			unsigned b46:1;
//			unsigned b47:1;
//			unsigned b48:1;
//			unsigned b49:1;
//			unsigned b50:1;
//			unsigned b51:1;
//			unsigned b52:1;
//			unsigned b53:1;
//			unsigned b54:1;
//			unsigned b55:1;
//			unsigned b56:1;
//			unsigned b57:1;
//			unsigned b58:1;
//			unsigned b59:1;
//			unsigned b60:1;
//			unsigned b61:1;
//			unsigned b62:1;
//			unsigned b63:1;
//			unsigned b64:1;
//			unsigned b65:1;
//			unsigned b66:1;
//			unsigned b67:1;
//			unsigned b68:1;
//			unsigned b69:1;
//			unsigned b70:1;
//			unsigned b71:1;
//			unsigned b72:1;
//			unsigned b73:1;
//			unsigned b74:1;
//			unsigned b75:1;
//			unsigned b76:1;
//			unsigned b77:1;
//			unsigned b78:1;
//			unsigned b79:1;
//			unsigned b80:1;
//			unsigned b81:1;
//			unsigned b82:1;
//			unsigned b83:1;
//			unsigned b84:1;
//			unsigned b85:1;
//			unsigned b86:1;
//			unsigned b87:1;
//			unsigned b88:1;
//			unsigned b89:1;
//			unsigned b90:1;
//			unsigned b91:1;
//			unsigned b92:1;
//			unsigned b93:1;
//			unsigned b94:1;
//			unsigned b95:1;
//			unsigned b96:1;
//			unsigned b97:1;
//			unsigned b98:1;
//			unsigned b99:1;
//			unsigned b100:1;
//			unsigned b101:1;
//			unsigned b102:1;
//			unsigned b103:1;
//			unsigned b104:1;
//			unsigned b105:1;
//			unsigned b106:1;
//			unsigned b107:1;
//			unsigned b108:1;
//			unsigned b109:1;
//			unsigned b110:1;
//			unsigned b111:1;
//			unsigned b112:1;
//			unsigned b113:1;
//			unsigned b114:1;
//			unsigned b115:1;
//			unsigned b116:1;
//			unsigned b117:1;
//			unsigned b118:1;
//			unsigned b119:1;
//			unsigned b120:1;
//			unsigned b121:1;
//			unsigned b122:1;
//			unsigned b123:1;
//			unsigned b124:1;
//			unsigned b125:1;
//			unsigned b126:1;
//			unsigned b127:1;
//			unsigned b128:1;
//			unsigned b129:1;
//			unsigned b130:1;
//			unsigned b131:1;
//			unsigned b132:1;
//			unsigned b133:1;
//			unsigned b134:1;
//			unsigned b135:1;
//			unsigned b136:1;
//			unsigned b137:1;
//			unsigned b138:1;
//			unsigned b139:1;
//			unsigned b140:1;
//			unsigned b141:1;
//			unsigned b142:1;
//			unsigned b143:1;
//			unsigned b144:1;
//			unsigned b145:1;
//			unsigned b146:1;
//			unsigned b147:1;
//			unsigned b148:1;
//			unsigned b149:1;
//			unsigned b150:1;
//			unsigned b151:1;
//			unsigned b152:1;
//			unsigned b153:1;
//			unsigned b154:1;
//			unsigned b155:1;
//			unsigned b156:1;
//			unsigned b157:1;
//			unsigned b158:1;
//			unsigned b159:1;
//			unsigned b160:1;
//			unsigned b161:1;
//			unsigned b162:1;
//			unsigned b163:1;
//			unsigned b164:1;
//			unsigned b165:1;
//			unsigned b166:1;
//			unsigned b167:1;
//			unsigned b168:1;
//			unsigned b169:1;
//			unsigned b170:1;
//			unsigned b171:1;
//			unsigned b172:1;
//			unsigned b173:1;
//			unsigned b174:1;
//			unsigned b175:1;
//			unsigned b176:1;
//			unsigned b177:1;
//			unsigned b178:1;
//			unsigned b179:1;
//			unsigned b180:1;
//			unsigned b181:1;
//			unsigned b182:1;
//			unsigned b183:1;
//			unsigned b184:1;
//			unsigned b185:1;
//			unsigned b186:1;
//			unsigned b187:1;
//			unsigned b188:1;
//			unsigned b189:1;
//			unsigned b190:1;
//			unsigned b191:1;
//		}bits;
//	}u;
//} recv_frame_t;
//
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
		struct fields_29 {
			unsigned 	sof:1;
			unsigned 	id:29;
			unsigned	rtr:1;
			unsigned 	r0:1;
			unsigned 	r1:1;
			unsigned 	dlc:4;
			unsigned 	crc:16;
			unsigned	ack:2;
			unsigned 	eof:7;
			unsigned 	inter:3;
		} fields_29;
	} fixed_field;
	uint8_t data[8];
	uint8_t size;
	uint8_t wr_ind;
	uint8_t wr_off;
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

//void read_recv_buffer(recv_buffer_t recv_buffer, recv_frame_t * data, uint8_t nframes);


#endif
