#include "delay.h"
#include "SHT2x.h"
#include "iic1.h"
#include "common.h"

float  Temperature = 0.0f;
float  Humidity = 0.0f;

/************************/
//初始化IIC
void SHT2x_Init(void)
{					   
	SHT2xSoftReset();
}

/*
*	多项式：X8+X5+X4+1
*	Poly：0x131
*/
u8 crc8_chk_value(u8 *buf, u8 len)
{
	u8 crc;
	u8 i;
	
	crc = 0;
	while(len --)
	{
		crc ^= *buf ++;
		for(i = 0;i < 8;i ++)
		{
			if(crc & 0x80)
			{
				crc = (crc << 1) ^ 0x131;
			}
			else 
				crc <<= 1;
		}
	}
	return crc;
}

//读取温度
float Sht2xReadTemperature(void)
{
	u8 times = 0;
	static float wendu_val,tt;
	u32  aaa=0;
	u8 ack;
	u8 tem[2],crc8,crc8_c;
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_IIC1, portMAX_DELAY);
	}
	
	IIC1_Start(); 
	
	IIC1_Send_Byte(0x80);
	IIC1_Wait_Ack();

	IIC1_Send_Byte(0xf3);
	IIC1_Wait_Ack();
    
	do
	{
		delay_us(500);               
		IIC1_Start();
		IIC1_Send_Byte(0x81);
		ack = IIC1_Wait_Ack();
		
		times ++;
		if(times >= 100)
		{
			if(xSchedulerRunning == 1)
			{
				xSemaphoreGive(xMutex_IIC1);
			}
			
			return wendu_val;
		}
	}
	while(ack);

	tem[0] = IIC1_Read_Byte(1);
	tem[1] = IIC1_Read_Byte(1);
	crc8 = IIC1_Read_Byte(0);
	IIC1_Stop();
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_IIC1);
	}
	
	crc8_c = crc8_chk_value(tem,2);
	
	if(crc8 == crc8_c)
	{
		aaa = (aaa|tem[0])*256+tem[1];
		aaa = aaa & 0xfffc;
		tt=(float)( aaa * 0.02681);
		wendu_val =( tt -468.5)/10;
	}
	
	return(wendu_val);
}

//读取湿度
float Sht2xReadHumidity(void)
{
	u8 times = 0;
	static float sdd,sd;
	u32  aaa=0;
	u8 ack;
	u8 tem[2],crc8,crc8_c;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_IIC1, portMAX_DELAY);
	}
	
	IIC1_Start();

	IIC1_Send_Byte(0x80);
	IIC1_Wait_Ack();

	IIC1_Send_Byte(0xf5);
	IIC1_Wait_Ack();

	do
	{
		delay_us(500);
		IIC1_Start();
		IIC1_Send_Byte(0x81); 
		ack = IIC1_Wait_Ack();
		
		times ++;
		if(times >= 100)
		{
			if(xSchedulerRunning == 1)
			{
				xSemaphoreGive(xMutex_IIC1);
			}
			
			return sdd;
		}
	}
	while(ack==1);

	tem[0] = IIC1_Read_Byte(1);
	tem[1] = IIC1_Read_Byte(1);
	crc8 = IIC1_Read_Byte(0);
	IIC1_Stop();
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_IIC1);
	}
	
	crc8_c = crc8_chk_value(tem,2);
	
	if(crc8 == crc8_c)
	{
		aaa = (aaa|tem[0])*256+tem[1];
		aaa = aaa & 0xff00;
		sd=(float)(aaa * 0.00190735);
		sdd=(sd-6);
	}
	
	return sdd;
}

void Sht2xReadSerialNumber(u8 *buf)
{
	/* Read from memory location 1 */
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_IIC1, portMAX_DELAY);
	}
	
	IIC1_Start();
	IIC1_Send_Byte(0x80); //I2C address
	IIC1_Wait_Ack();
	IIC1_Send_Byte(0xFA); //Command for readout on-chip memory
	IIC1_Wait_Ack();
	IIC1_Send_Byte(0x0F); //on-chip memory address
	IIC1_Wait_Ack();
	IIC1_Start();
	IIC1_Send_Byte(0x81); //I2C address
	IIC1_Wait_Ack();
	buf[5] = IIC1_Read_Byte(1); //Read SNB_3
	IIC1_Read_Byte(1); //Read CRC SNB_3 (CRC is not analyzed)
	buf[4] = IIC1_Read_Byte(1); //Read SNB_2
	IIC1_Read_Byte(1); //Read CRC SNB_2 (CRC is not analyzed)
	buf[3] = IIC1_Read_Byte(1); //Read SNB_1
	IIC1_Read_Byte(1); //Read CRC SNB_1 (CRC is not analyzed)
	buf[2] = IIC1_Read_Byte(1); //Read SNB_0
	IIC1_Read_Byte(0); //Read CRC SNB_0 (CRC is not analyzed)
	IIC1_Stop();

	/* Read from memory location 2 */
	IIC1_Start();
	IIC1_Send_Byte(0x80); //I2C address
	IIC1_Wait_Ack();
	IIC1_Send_Byte(0xFC); //Command for readout on-chip memory
	IIC1_Wait_Ack();
	IIC1_Send_Byte(0xC9); //on-chip memory address
	IIC1_Wait_Ack();
	IIC1_Start();
	IIC1_Send_Byte(0x81); //I2C address
	IIC1_Wait_Ack();
	buf[1] = IIC1_Read_Byte(1); //Read SNC_1
	buf[0] = IIC1_Read_Byte(1); //Read SNC_0
	IIC1_Read_Byte(1); //Read CRC SNC0/1 (CRC is not analyzed)
	buf[7] = IIC1_Read_Byte(1); //Read SNA_1
	buf[6] = IIC1_Read_Byte(1); //Read SNA_0
	IIC1_Read_Byte(0); //Read CRC SNA0/1 (CRC is not analyzed)
	IIC1_Stop();
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_IIC1);
	}
}

void SHT2xSoftReset(void)
{
	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_IIC1, portMAX_DELAY);
	}
	
    IIC1_Start();
    IIC1_Send_Byte(0x80);
	IIC1_Wait_Ack();
    IIC1_Send_Byte(0xFE);
	IIC1_Wait_Ack();
    IIC1_Stop();
	
	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_IIC1);
	}
}

