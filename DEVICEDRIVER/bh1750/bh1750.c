#include "bh1750.h"
#include "iic2.h"
#include "delay.h"

float Illumination = 0.0f;

void Bh1750_Init(void)
{
	IIC2_Init();
	
	bh_data_send(BHPowOn);
    bh_data_send(BHReset);
    bh_data_send(BHModeH2);
	delay_ms(180);
}

void bh_data_send(u8 command)
{
	u8 i = 0;
    do
	{
		IIC2_Start();
		IIC2_Send_Byte(BHAddWrite);
		
		i ++;
		
		if(i >= 20)
		{
			break;
		}
	}
	while(IIC2_Wait_Ack());
	
	IIC2_Send_Byte(command);
	IIC2_Wait_Ack();
	IIC2_Stop();
}

u16 bh_data_read(void)
{
	u16 light;
	
	IIC2_Start();
	IIC2_Send_Byte(BHAddRead);
	IIC2_Wait_Ack();
	light = IIC2_Read_Byte(1);
	light = light << 8;
	light += 0x00ff & IIC2_Read_Byte(0);
	IIC2_Stop();
	
	return light;
}

float Bh1750ReadIllumination(void)
{
	float illumination = 0.0f;
	
	illumination = (float)bh_data_read() / 1.2f;
	
	return illumination;
}


































