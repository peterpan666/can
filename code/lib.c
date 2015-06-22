#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

#include "lib.h"

//void read_recv_buffer(recv_buffer_t recv_buffer, recv_frame_t * data, uint8_t nframes){
//	uint8_t i, read;
//	for (i = 0; i < nframes; i++) {
//		read = recv_buffer.read--;
//		data[i] = recv_buffer.buffer[read];
//	}
//}
//
//void read_decd_buffer(decd_buffer_t decd_buffer, decd_frame_t * data, uint8_t nframes){
//	uint8_t i, read;
//	for (i = 0; i < nframes; i++) {
//		read = decd_buffer.read--;
//		data[i] = decd_buffer.buffer[read];
//	}
//}

uint8_t create_mask(uint8_t s, uint8_t e){
	uint8_t m = 0;
	uint8_t i;
	for (i=s ; i <= e; i++)
		m |= 1 << i;
	return m;
}

void write_bit_in_frame(decd_frame_t * frame, uint8_t bit_val) {
	pT_frame_t p;
	p.ind = frame->wr_ind;
	p.off = frame->wr_off;
	write_tab(frame->fixed_field.tab, p, bit_val);
	if (frame->wr_off > 6){
		frame->wr_off = 0;
		frame->wr_ind ++;
		frame->size ++;
	} else {
		frame->wr_off ++;
	}
}

void get_pT_from_bit_pos(uint8_t pos, pT_frame_t * p) {
	p->ind = pos/8;
	p->off = pos%8;
}

uint8_t read_bit_in_frame(decd_frame_t* frame, pT_frame_t p) {
	return read_tab(frame->fixed_field.tab, p);
}

void write_tab(uint8_t* tab, pT_frame_t p, uint8_t bit_val) {
	uint8_t mask;
	mask = 1 << p.off;
	if (bit_val == 0) {
		mask = ~ mask;
		tab[p.ind] &= mask;
	} else {
		tab[p.ind] |= mask;
	}
}

uint8_t read_tab(uint8_t * tab, pT_frame_t p){
	return (tab[p.ind] >> p.off) & 0x1;
}

uint32_t read_n_bit(uint8_t * tab, uint8_t spos, uint8_t nbits){
	uint32_t mask, val = 0;
	uint8_t i, bit_val;
	pT_frame_t pos;
	for (i = 0; i < nbits; i++) {
		get_pT_from_bit_pos(spos + i, &pos);
		bit_val = read_tab(tab, pos);
		mask = 1 << i;
		if (bit_val == 0) {
			mask = ~ mask;
			val &= mask;
		} else {
			val |= mask;
		}
	}
	return val;
}

//void generate_bit_stuffing(uint8_t data_in[24], uint8_t data_out[24], uint32_t nbits){
//	int bit_cnt = 0;
//	int out_cnt = 0;
//	uint8_t ind, shift, stuff_bit, current_byte_mask,n_prec_byte, data;
//	for(bit_cnt = 4; bit_cnt < nbits; bit_cnt += 5){
//		ind = bit_cnt/8;
//		shift = bit_cnt%8;
//		stuff_bit = ((data_in[ind] >> shift) & 0x1) == 0;
//		if (shift < 4) {
//			n_prec_byte = 4 - shift;
//			current_byte_mask = create_mask(0, shift);
//			data = (data_in[ind-1] >> (8 - n_prec_byte)) | ((data_in[ind] & current_byte_mask) << n_prec_byte) | (stuff_bit << 5);
//		} else {
//			data = ((data_in[ind] >> (shift - 4)) & 0b11111) | (stuff_bit << 5);
//		}
//	}
//}
