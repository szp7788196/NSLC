#ifndef __BH1750_H
#define __BH1750_H

#include "stm32f10x.h"


#define BHAddWrite     0x46
#define BHAddRead      0x47 
#define BHPowDown      0x00
#define BHPowOn        0x01
#define BHReset        0x07
#define BHModeH1       0x10
#define BHModeH2       0x11
#define BHModeL        0x13
#define BHSigModeH     0x20
#define BHSigModeH2    0x21
#define BHSigModeL     0x23


extern float Illumination;


void Bh1750_Init(void);
void bh_data_send(u8 command);
u16 bh_data_read(void);
float Bh1750ReadIllumination(void);








































#endif
