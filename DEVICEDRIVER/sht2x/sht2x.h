#ifndef __SHT2x_H
#define __SHT2x_H

#include "sys.h"



extern float  Temperature;
extern float  Humidity;

void SHT2x_Init(void);                //初始化IIC的IO口				 

void SHT2xSoftReset(void);

u8 crc8_chk_value(u8 *buf, u8 len);
float Sht2xReadTemperature(void);
float Sht2xReadHumidity(void);

void Sht2xReaderialNumber(u8 *buf);


#endif
