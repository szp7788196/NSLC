#include "att7059x.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

//计算校验码
u8 CalAtt7059CheckSum(u8 *buf, u8 len)
{
	u8 i = 0;
	u8 check_sum = 0;

	for(i = 0; i < len; i ++)
	{
		check_sum += buf[i];
	}

	check_sum = ~check_sum;

	return check_sum;
}

//向7059写入数据
u8 Att7059xWriteOperate(u8 add, u16 data)
{
	u8 ret = 0;
	u8 i = 0;
	u8 send_len = 0;
	u8 send_buf[6];

	send_buf[send_len] = 0x6A;
	send_len ++;
	send_buf[send_len] = 0x80 | add;
	send_len ++;
	send_buf[send_len] = (u8)(data >> 8);
	send_len ++;
	send_buf[send_len] = (u8)(data & 0x00FF);
	send_len ++;
	send_buf[send_len] = 0;
	send_len ++;

	send_buf[send_len] = CalAtt7059CheckSum(send_buf, send_len);

	UsartSendString(USART3,send_buf, send_len);

	i = 100;
	while(i --)
	{
		delay_ms(10);

		if(Usart3RecvEnd == 0xAA)
		{
			Usart3RecvEnd = 0;

			if(Usart3FrameLen == 1)
			{
				Usart3FrameLen = 0;

				if(Usart3RxBuf[0] == ACK_OK)
				{
					i = 0;
					ret = 1;
				}
			}
		}
	}

	return ret;
}

//从7059中读取数据
u8 Att7059xReadOperate(u8 add, u32 *data)
{
	u8 ret = 0;
	u8 i = 0;
	u8 send_len = 0;
	u8 send_buf[6];
	u8 check_sum = 0;

	*data = 0;

	send_buf[send_len] = 0x6A;
	send_len ++;
	send_buf[send_len] = 0x00 | add;
	send_len ++;

	UsartSendString(USART3,send_buf, send_len);

	i = 100;
	while(i --)
	{
		delay_ms(10);

		if(Usart3RecvEnd == 0xAA)
		{
			Usart3RecvEnd = 0;

			if(Usart3FrameLen == 4)
			{
				memcpy(&send_buf[2],Usart3RxBuf,Usart3FrameLen);

				check_sum = CalAtt7059CheckSum(send_buf, 5);

				if(check_sum == Usart3RxBuf[Usart3FrameLen - 1])
				{
					*data = (((u32)Usart3RxBuf[0]) << 16) + \
							(((u32)Usart3RxBuf[1]) << 8) + \
							(u32)Usart3RxBuf[2];

					i = 0;
					ret = 1;
				}

				Usart3FrameLen = 0;
			}
		}
	}

	return ret;
}

//获取电流通道1的ADC采样数据
s32 Att7059xGetCurrent1ADCValue(void)
{
	static s32 adc_value = 0;
	s32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x00, (u32 *)&value);

	if(ret == 1)								//将补码转换为源码
	{
		if(value & (1 << 23))
		{
			value = value - 0x00FFFFFF;
		}
	}

	return adc_value;
}

//获取电压通道的ADC采样数据
s32 Att7059xGetVoltageADCValue(void)
{
	static s32 adc_value = 0;
	s32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x02, (u32 *)&value);

	if(ret == 1)								//将补码转换为源码
	{
		if(value & (1 << 23))
		{
			value = value - 0x00FFFFFF;
		}
	}

	return adc_value;
}

//获取电流通道1的有效值
float Att7059xGetCurrent1(void)
{
	static u32 current = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x06, &value);

	if(ret == 1)
	{
		current = (float)value * CURRENT_RATIO;
	}

	return current;
}

//获取电压通道的有效值
float Att7059xGetVoltage(void)
{
	static u32 voltage = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x08, &value);

	if(ret == 1)
	{
		voltage = (float)value * VOLTAGE_RATIO;
	}

	return voltage;
}

//获取电压通道的频率
float Att7059xGetVoltageFreq(void)
{
	static float freq_value = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x09, &value);

	if(ret == 1)
	{
		freq_value = 1000000.0f / 2.0f / (float)value;
	}

	return freq_value;
}

//获取通道1的有功功率
float Att7059xGetChannel1PowerP(void)
{
	static float power_p = 0;
	s32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0A, (u32 *)&value);

	if(ret == 1)								//将补码转换为源码
	{
		if(value & (1 << 23))
		{
			value = value - 0x00FFFFFF;
		}

		power_p = (float)value * POWER_RATIO;
	}

	return power_p;
}

//获取通道1的无功功率
float Att7059xGetChannel1PowerQ(void)
{
	static float power_q = 0;
	s32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0B, (u32 *)&value);

	if(ret == 1)								//将补码转换为源码
	{
		if(value & (1 << 23))
		{
			value = value - 0x00FFFFFF;
		}

		power_q = (float)value * POWER_RATIO;
	}

	return power_q;
}

//获取通道1的视在功率
float Att7059xGetChannel1PowerS(void)
{
	static float power_s = 0;
	s32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0C, (u32 *)&value);

	if(ret == 1)								//将补码转换为源码
	{
		if(value & (1 << 23))
		{
			value = value - 0x00FFFFFF;
		}

		power_s = (float)value * POWER_RATIO;
	}

	return power_s;
}

//获取有功能量 单位千瓦
float Att7059xGetEnergyP(void)
{
	static float energy_p = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0D, &value);

	if(ret == 1)
	{
		energy_p = (float)value / ELECTRIC_ENERGY_METER_CONSTANT;
	}

	return energy_p;
}

//获取无功能量 单位千瓦
float Att7059xGetEnergyQ(void)
{
	static float energy_q = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0E, &value);

	if(ret == 1)
	{
		energy_q = (float)value / ELECTRIC_ENERGY_METER_CONSTANT;
	}

	return energy_q;
}

//获取视在能量 单位千瓦
float Att7059xGetEnergyS(void)
{
	static float energy_s = 0;
	u32 value = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x0F, &value);

	if(ret == 1)
	{
		energy_s = (float)value / ELECTRIC_ENERGY_METER_CONSTANT;
	}

	return energy_s;
}

//获取电压通道的ADC最大采样数据
s32 Att7059xGetMaxVoltageADCValue(void)
{
	static s32 adc_value = 0;
	u32 value = 0;
	u8 sign = 0;
	u8 ret = 0;

	ret = Att7059xReadOperate(0x12, &value);

	if(ret == 1)								//将补码转换为源码
	{
		value |= 0xFF000000;					//高8位强制为1

		if(value & (1 << 21))
		{
			sign = 1;							//负数
		}
		else
		{
			sign = 0;							//正数
		}

		value |= 0x00E00000;					//bit21~23强制为1

		value = ~value;

		if(sign)
		{
			value |= ((u32)1 << 31);			//负数
		}
		else
		{
			value &= ~((u32)1 << 31);			//正数
		}

		adc_value = (s32)value;
	}

	return adc_value;
}
















































