#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "lib.h"
#include "decode.h"
#include "globals.h"

//permet d'executer le process "timer_decode" avec un temps de pause
uint8_t timer_decode = 0;

void decode_timer(void) {
	//Gestion de la variable temporelle de la tache decode
	if(timer_decode != 0){
		timer_decode--;
	}
}

void decode_task(void) {
	//Machine d'etat de la tache decode
	if(!timer_decode){
		timer_decode = conv_bdt(1);//Base de temps de repetition de la tache decode
	}
	// Decodage de la trame pointee par read dans le buffer de reception
	//destuf(&GLB_recv_buffer.buffer[GLB_recv_buffer.read++], &GLB_decd_buffer.buffer[GLB_decd_buffer.write++]);
	parse_frame(&GLB_decd_buffer.buffer[GLB_decd_buffer.read++]);
}

void decode_init(void) {
	GLB_decd_buffer.buffer[0].fixed_field.tab[0] = 0b01111000;
	GLB_decd_buffer.buffer[0].fixed_field.tab[1] = 0b00001111;
	GLB_decd_buffer.buffer[0].fixed_field.tab[2] = 0b01111010;
	GLB_decd_buffer.buffer[0].fixed_field.tab[3] = 0b11110111;
	GLB_decd_buffer.buffer[0].fixed_field.tab[4] = 0b01101101;
	GLB_decd_buffer.buffer[0].fixed_field.tab[5] = 0b11110101;
	GLB_decd_buffer.buffer[0].fixed_field.tab[6] = 0b11110110;
	GLB_decd_buffer.buffer[0].fixed_field.tab[7] = 0b11010101;
	GLB_decd_buffer.buffer[0].fixed_field.tab[8] = 0b11101101;
	GLB_decd_buffer.buffer[0].fixed_field.tab[9] = 0b01111111;
	GLB_decd_buffer.buffer[0].size = 10;
}

void destuf (frame_t* in, decd_frame_t* out) {
	uint8_t current_bit, bit_ref, is_stuff_bit = 0, cnt = 0, limit = 5, i, j, end_bit = 8;
	// On parcours le tableau de 24 octet contenant la trame
	current_bit = in->buffer[0] & 0x1;
	bit_ref = current_bit;
	pT_frame_t eos;
	eos = compute_eos(in);
	for (i = 0; i <= eos.ind; i++){
		if (i == eos.ind)
			end_bit = eos.off+1;
		// On parcours chaque bit de l'octet et on le compare au bit de reference
		for (j = 0; j < end_bit; j++) {
			// On recuppere le bit courant
			current_bit = ((in->buffer[i]>>j) & (0x1));
			if (is_stuff_bit == 0) {
				// Si le bit courant est egal au bit de ref on incremente le compteur sinon on le remet a 0
				if (current_bit == bit_ref) cnt ++;
				else {
					bit_ref = current_bit;
					cnt = 1;
				}
				// Si le compteur a atteint la limite on recopie le bit dans la trame sans bitstuffing
				// on remet le compteur a 0 et on met un flag a 1 pour ne pas copier le prochain bit
				// on change la valeur limite si on etait dans la premiere detection de bit de stuff
				if (cnt >= limit) {
					is_stuff_bit = 1;
				}
				write_bit_in_frame(out, current_bit);
			} else {
				is_stuff_bit = 0;
				cnt = 1;
				bit_ref = current_bit;
			}
		}
	}
}

pT_frame_t compute_eos(frame_t * f) {
	pT_frame_t p;
	// On initialise la position avec celle du dernier bit du dernier octet
	uint8_t pos = 8 * f->size - 1;
	// On recupere la valeur du bit correspondant a la position
	uint8_t bit = (f->buffer[f->size - 1] >> pos%8) & 0x1;
	// Si ce bit vaut 1 alors la position correpond a la fin de la trame
	// Sinon on entre dans une boucle et on parcours le dernier octet jusqu'a rencontrer un bit a 1
	// Ce bit est le dernier bit de la trame
	while (bit != 1) {
		pos --;
		bit = (f->buffer[f->size - 1] >> pos%8) & 0x1;
	}
	// En sortie de boucle la position correspond a celle du dernier bit de la trame
	// Le bit stuffing n'est plus calcule a partir du 13e bit en partant de la fin de trame
	// On calcule donc le pointeur de trame de la fin de stuffing a partir de la position - 13 bits
	get_pT_from_bit_pos(pos - 13, &p);

	return p;
}

void parse_frame (decd_frame_t * frame) {
	uint8_t dlc = frame->fixed_field.fields_11.dlc;
	get_data(frame,dlc,frame->data);
	remove_data(frame,dlc);
}

void get_data(decd_frame_t * frame, uint8_t dlc, uint8_t data[8]) {
	pT_frame_t pT_pos, pT_pos_in_frame;
	uint8_t pos = 0;
	uint8_t pos_in_frame = 19;
	for(pos = 0; pos < dlc*8; pos ++) {
		get_pT_from_bit_pos(pos, &pT_pos);
		get_pT_from_bit_pos(pos_in_frame ++, &pT_pos_in_frame);
		write_tab(data, pT_pos, read_bit_in_frame(frame,pT_pos_in_frame));
	}
}

void remove_data(decd_frame_t * frame, uint8_t dlc) {
	uint8_t rd_pos = 19 + 8*dlc;
	frame->fixed_field.fields_11.crc = (uint16_t) read_n_bit(frame->fixed_field.tab, rd_pos , 16);
	rd_pos += 16;
	frame->fixed_field.fields_11.ack = (unsigned) read_n_bit(frame->fixed_field.tab, rd_pos , 2);
	rd_pos += 2;
	frame->fixed_field.fields_11.eof = (unsigned) read_n_bit(frame->fixed_field.tab, rd_pos , 7);
	rd_pos += 7;
	frame->fixed_field.fields_11.inter = (unsigned) read_n_bit(frame->fixed_field.tab, rd_pos , 3);
}

