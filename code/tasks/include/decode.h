#ifndef _DECODE_H_
#define _DECODE_H_

void decode_timer(void);
void decode_task(void);
void decode_init(void);
void destuf (frame_t* in, decd_frame_t* out);
pT_frame_t compute_eos(frame_t * f);
void parse_frame (decd_frame_t * frame);
void get_data(decd_frame_t * frame, uint8_t dlc, uint8_t data[8]);
void remove_data(decd_frame_t * frame, uint8_t dlc);
#endif

#define IDLE	0
#define DESTUF	1
#define PARSE 	2
