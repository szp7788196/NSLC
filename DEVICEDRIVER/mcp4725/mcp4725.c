#include "mcp4725.h"
#include "iic2.h"


void Mcp4725Init(void)
{
	IIC2_Init();
}


void WriteMcp4725(u16 value)
{
	u16 data_h = 0;
	u16 data_l = 0;

	if(value > 4095)
	{
		return;
	}

	data_h = (u8)(value >> 8) & 0x0F;
	data_l = (u8)(value & 0x00FF);

	IIC2_Start();

	IIC2_Send_Byte(MCP4725_ADD);
	IIC2_Wait_Ack();

	IIC2_Send_Byte(data_h);
	IIC2_Wait_Ack();

	IIC2_Send_Byte(data_l);
	IIC2_Wait_Ack();

	IIC2_Stop();
}

//设置MCP4725的输出电压
void Mcp4725SetOutPutVoltage(float voltage)
{
	u16 value = 0;

	if(voltage > 5.0f)
	{
		return;
	}

	value = (u16)((voltage / 5.0f) * 4095.0f + 0.5f);

	WriteMcp4725(value);
}




































