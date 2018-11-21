#ifndef __DALI_H
#define __DALI_H

#include "sys.h"


#define NO_ACTION 			0
#define SENDING_DATA 		1
#define RECEIVING_DATA 		2
#define ERR 				3


#define DALIIN_INVERT		0
#define DALIOUT_INVERT		0



void DALI_Init(void);
u8 dali_get_flag(void);
void dali_receive_data(void);
void dali_send_data(u8 address, u8 command);
bool dali_get_DALIIN(void);
void dali_set_DALIOUT(bool pin_value);
bool dali_get_DALIOUT(void);
void dali_receive_tick(void);
void dali_send_tick(void);


































#endif
