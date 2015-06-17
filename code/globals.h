#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "lib.h"

//buffer de reception (taille fixe de 256 trame)
recv_buffer_t GLB_recv_buffer = {0};
decd_buffer_t GLB_decd_buffer = {0};


 //************* state machine ******************
// unsigned char GLB_timer_state_machine=0;
 //unsigned char GLB_state_send_machine=0;
// unsigned int GLB_timer_send_machine=1;
// unsigned int GLB_reload_send_machine=1;
 unsigned char GLB_send_finish=1;
 unsigned char *GLB_message;
// unsigned char distance[]="distance=";
// unsigned char cm[]="cm\n\r";
// unsigned char GLB_DISPLAY_ON = 1;


 //************ liaison RS232 ******************

 // unsigned char GLB_received_command=0;
 // unsigned char GLB_received_parameter=0;
  unsigned char GLB_current_character=0;
  unsigned char GLB_data_to_send;
 // unsigned char GLB_STANDBY_ON = 0;
 // int GLB_indice_message = 0;
 // int GLB_ligne_message = 0;

#endif
