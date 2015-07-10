#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "lib.h"
#include "decode.h"

extern recv_buffer_t GLB_recv_buffer;
extern decd_buffer_t GLB_decd_buffer;

/* Erreurs detectees :
 *
 * bit stuffing ....... ok
 * dlc ................ ok
 * crc ................
 * r0 ................. ok
 * id ................. ok
 *
 */

//permet d'executer le process "timer_decode" avec un temps de pause
static uint8_t timer_decode = 0, status = IDLE;
static uint8_t id_error, nb_id = 4;
static uint16_t tab_id[8] = {0x500,2,0x7bc,5,6,7,8};

void decode_timer(void) {
	//Gestion de la variable temporelle de la tache decode
	if(timer_decode != 0){
		timer_decode--;
	}
}

void decode_task(void) {
	//Machine d'etat de la tache decode
	if(timer_decode)
		return;

	timer_decode = conv_bdt(20);//Base de temps de repetition de la tache decode

	switch (status) {

	case IDLE :
		if (GLB_recv_buffer.read != GLB_recv_buffer.write)
			status = DESTUF;
		break;

	case DESTUF :
		// Destuffing de la trame pointee par read dans le buffer de reception et copie dans le buffer de decodage
		destuf(&GLB_recv_buffer.buffer[GLB_recv_buffer.read++], &GLB_decd_buffer.buffer[GLB_decd_buffer.write]);
		status = PARSE;
		break;

	case PARSE :
		// Decodage de la trame directement dans le buffer de reception
		parse_frame(&GLB_decd_buffer.buffer[GLB_decd_buffer.write++]);
		status = IDLE;
		break;

	}
}

void decode_init(void) {
   /*	Trame de test avec bitstuffing sans erreur
	*	sof  	: 0
	*	id 		: 0x7bc
	*	rtr	 	: 0
	*	r0 		: 1
	*	r1 		: 0
	*	dlc 	: 0x4
	*	data 	: 0xdeadbeef
	*	crc 	: 0xbabe
	*	ack 	: 0b01 (1)
	*	eof 	: 0x7f
	*	inter 	: 0b111 (0x7) */

	GLB_recv_buffer.buffer[0].buffer[0] = 0b01111000;
	GLB_recv_buffer.buffer[0].buffer[1] = 0b00001111;
	GLB_recv_buffer.buffer[0].buffer[2] = 0b11110110;
	GLB_recv_buffer.buffer[0].buffer[3] = 0b11101110;
	GLB_recv_buffer.buffer[0].buffer[4] = 0b10110011;
	GLB_recv_buffer.buffer[0].buffer[5] = 0b11010101;
	GLB_recv_buffer.buffer[0].buffer[6] = 0b11011011;
	GLB_recv_buffer.buffer[0].buffer[7] = 0b10100111;
	GLB_recv_buffer.buffer[0].buffer[8] = 0b01101110;
	GLB_recv_buffer.buffer[0].buffer[9] = 0b11111111;
	GLB_recv_buffer.buffer[0].buffer[10] = 0b00000011;
	GLB_recv_buffer.buffer[0].size = 11;
	GLB_recv_buffer.write++;

   /*	Trame de test avec bitstuffing
    *   /!\ erreur de bit stuffing (bit 34)/!\
	*	sof  	: 0
	*	id 		: 0x7bc
	*	rtr	 	: 0
	*	r0 		: 0
	*	r1 		: 0
	*	dlc 	: 0x4
	*	data 	: 0xdeadbeef
	*	crc 	: 0xbabe
	*	ack 	: 0b01 (1)
	*	eof 	: 0x7f
	*	inter 	: 0b111 (0x7) */

	GLB_recv_buffer.buffer[1].buffer[0] = 0b01111000;
	GLB_recv_buffer.buffer[1].buffer[1] = 0b00001111;
	GLB_recv_buffer.buffer[1].buffer[2] = 0b11110110;
	GLB_recv_buffer.buffer[1].buffer[3] = 0b11101110;
	GLB_recv_buffer.buffer[1].buffer[4] = 0b10110111;
	GLB_recv_buffer.buffer[1].buffer[5] = 0b11010101;
	GLB_recv_buffer.buffer[1].buffer[6] = 0b11011011;
	GLB_recv_buffer.buffer[1].buffer[7] = 0b10100111;
	GLB_recv_buffer.buffer[1].buffer[8] = 0b01101110;
	GLB_recv_buffer.buffer[1].buffer[9] = 0b11111111;
	GLB_recv_buffer.buffer[1].buffer[10] = 0b00000011;
	GLB_recv_buffer.buffer[1].size = 11;
	GLB_recv_buffer.write++;

   /*	Trame de test avec bitstuffing
	*   /!\ erreur de r0 /!\
	*	sof  	: 0
	*	id 		: 0x7bc
	*	rtr	 	: 0
	*	r0 		: 1
	*	r1 		: 0
	*	dlc 	: 0x4
	*	data 	: 0xdeadbeef
	*	crc 	: 0xbabe
	*	ack 	: 0b01 (1)
	*	eof 	: 0x7f
	*	inter 	: 0b111 (0x7) */

	GLB_recv_buffer.buffer[2].buffer[0] = 0b01111000;
	GLB_recv_buffer.buffer[2].buffer[1] = 0b00101111;
	GLB_recv_buffer.buffer[2].buffer[2] = 0b01111010;
	GLB_recv_buffer.buffer[2].buffer[3] = 0b11110111;
	GLB_recv_buffer.buffer[2].buffer[4] = 0b11011001;
	GLB_recv_buffer.buffer[2].buffer[5] = 0b11101010;
	GLB_recv_buffer.buffer[2].buffer[6] = 0b11101101;
	GLB_recv_buffer.buffer[2].buffer[7] = 0b01010011;
	GLB_recv_buffer.buffer[2].buffer[8] = 0b10110111;
	GLB_recv_buffer.buffer[2].buffer[9] = 0b11111111;
	GLB_recv_buffer.buffer[2].buffer[10] = 0b00000001;
	GLB_recv_buffer.buffer[2].size = 11;
	GLB_recv_buffer.write++;

   /*	Trame de test avec bitstuffing sans erreur
    *   /!\ erreur d'ID /!\
	*	sof  	: 0
	*	id 		: 0x5ac
	*	rtr	 	: 0
	*	r0 		: 1
	*	r1 		: 0
	*	dlc 	: 0x4
	*	data 	: 0xdeadbeef
	*	crc 	: 0xbabe
	*	ack 	: 0b01 (1)
	*	eof 	: 0x7f
	*	inter 	: 0b111 (0x7) */

	GLB_recv_buffer.buffer[3].buffer[0] = 0b01011000;
	GLB_recv_buffer.buffer[3].buffer[1] = 0b00001011;
	GLB_recv_buffer.buffer[3].buffer[2] = 0b11110110;
	GLB_recv_buffer.buffer[3].buffer[3] = 0b11101110;
	GLB_recv_buffer.buffer[3].buffer[4] = 0b10110011;
	GLB_recv_buffer.buffer[3].buffer[5] = 0b11010101;
	GLB_recv_buffer.buffer[3].buffer[6] = 0b11011011;
	GLB_recv_buffer.buffer[3].buffer[7] = 0b10100111;
	GLB_recv_buffer.buffer[3].buffer[8] = 0b01101110;
	GLB_recv_buffer.buffer[3].buffer[9] = 0b11111111;
	GLB_recv_buffer.buffer[3].buffer[10] = 0b00000011;
	GLB_recv_buffer.buffer[3].size = 11;
	GLB_recv_buffer.write++;

   /*	Trame de test avec bitstuffing sans erreur
    *   /!\ erreur de dlc /!\
	*	sof  	: 0
	*	id 		: 0x7bc
	*	rtr	 	: 0
	*	r0 		: 1
	*	r1 		: 0
	*	dlc 	: 0x5
	*	data 	: 0xdeadbeef
	*	crc 	: 0xbabe
	*	ack 	: 0b01 (1)
	*	eof 	: 0x7f
	*	inter 	: 0b111 (0x7) */

	GLB_recv_buffer.buffer[4].buffer[0] = 0b01111000;
	GLB_recv_buffer.buffer[4].buffer[1] = 0b10001111;
	GLB_recv_buffer.buffer[4].buffer[2] = 0b01111010;
	GLB_recv_buffer.buffer[4].buffer[3] = 0b11110111;
	GLB_recv_buffer.buffer[4].buffer[4] = 0b11011001;
	GLB_recv_buffer.buffer[4].buffer[5] = 0b11101010;
	GLB_recv_buffer.buffer[4].buffer[6] = 0b11101101;
	GLB_recv_buffer.buffer[4].buffer[7] = 0b01010011;
	GLB_recv_buffer.buffer[4].buffer[8] = 0b10110111;
	GLB_recv_buffer.buffer[4].buffer[9] = 0b11111111;
	GLB_recv_buffer.buffer[4].buffer[10] = 0b00000001;
	GLB_recv_buffer.buffer[4].size = 11;
	GLB_recv_buffer.write++;
}

void destuf (frame_t* in, decd_frame_t* out) {
	uint8_t current_bit, bit_ref, is_stuff_bit = 0, cnt = 0, limit = 5, i, j, end_bit = 8, scnt = 0;
	// On parcours le tableau de 24 octet contenant la trame
	current_bit = in->buffer[0] & 0x1;
	bit_ref = current_bit;
	pT_frame_t eos, pT_cp;
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
				scnt ++;
				// Detection d'erreur
				if(bit_ref == current_bit) {
					out->bs_error = 1;
					out->bs_error_pos = i*8+j;
				}
				is_stuff_bit = 0;
				cnt = 1;
				bit_ref = current_bit;
			}
		}
	}
	// On copie le reste de la trame (sans bitstuffing) a la suite
	for (i = eos.ind * 8 + eos.off + 1; i < eos.ind * 8 + eos.off + 13 + 1; i++) {
		get_pT_from_bit_pos(i, &pT_cp);
		write_bit_in_frame(out, read_tab(in->buffer, pT_cp));
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
	uint8_t real_dlc, dlc, i;

	if (frame->bs_error == 1) {
		return;
	}

	// Extraction des donnees de la trame
	dlc = frame->fixed_field.fields_11.dlc;
	get_data(frame, dlc, frame->data);

	// Supression des donnees de la trame
	remove_data(frame, dlc);

	/* ====== Detection d'erreurs ====== */
	// R0
	if (frame->fixed_field.fields_11.r0 != 0)
		frame->r0_error = 1;

	// dlc
	real_dlc = frame->wr_ind * 8 + frame->wr_off - 28 - 19;
	if (real_dlc != dlc * 8) {
		frame->dlc_error = 1;
		frame->real_dlc = real_dlc/8;
	}

	// id
	id_error = 1;
	for (i = 0 ; i < nb_id; i++) {
		if (frame->fixed_field.fields_11.id == tab_id[i])
			id_error = 0;
	}
	frame->id_error = id_error;
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

