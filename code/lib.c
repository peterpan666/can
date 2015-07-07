#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

#include "lib.h"

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

void crcInit(void)
{
    uint16_t remainder;
    uint16_t dividend;
    uint8_t bit;
    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */

uint16_t crcFast(uint8_t const message[], int nBytes)
{
    uint8_t data;
    uint16_t remainder = 0;
    int byte;

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder);

}   /* crcFast() */
