#ifndef __CD4051B_H
#define __CD4051B_H

#include "sys.h"

#define CD4051B_CS1_H		GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define CD4051B_CS1_L		GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define CD4051B_CS2_H		GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define CD4051B_CS2_L		GPIO_SetBits(GPIOB,GPIO_Pin_7)



void CD4051B_Init(void);
void cd4051b_set_channel(u8 ch);









































#endif
