#include "inventr.h"
#include "common.h"
#include "usart.h"
#include "pwm.h"
#include "mcp4725.h"

u8 InventrBusy = 0;
u8 InventrDisable = 0;

float InventrInPutCurrent = 0.0f;
float InventrInPutVoltage = 0.0f;
float InventrOutPutCurrent = 0.0f;
float InventrOutPutVoltage = 0.0f;

void RELAY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RELAY_ON;
}

//设置恒功率最大电流
u8 InventrSetMaxPowerCurrent(u8 percent)
{
	u8 ret = 0;
	u8 i = 0;
	u8 send_buf[8];
	
	if(InventrDisable == 0)
	{
		if(xSchedulerRunning == 1)
		{
			xSemaphoreTake(xMutex_INVENTR, portMAX_DELAY);
		}
		
		if(InventrBusy == 0)
		{
			InventrBusy = 1;
			
			memset(send_buf,0,8);
		
			send_buf[0] = 0x3A;
			send_buf[1] = 0x31;
			send_buf[2] = 0x00;
			send_buf[3] = 0x01;
			send_buf[4] = percent;
			send_buf[6] = 0x0D;
			send_buf[7] = 0x0A;
			
			for(i = 1; i <= 4; i ++)
			{
				send_buf[5] += send_buf[i];
			}
			
			UsartSendString(UART4,send_buf,8);
			
			i = 10;
			
			while(i --)
			{
				delay_ms(100);
				
				if(Usart4RecvEnd == 0xAA)
				{
					Usart4RecvEnd = 0;
					
					if(MyStrstr(Usart4RxBuf, send_buf, Usart4FrameLen, 8) != 0xFFFF)
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
					}
					else
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
						
						i = 1;
						ret = 1;
					}
				}
			}
			
			InventrBusy = 0;
		}
		
		if(xSchedulerRunning == 1)
		{
			xSemaphoreGive(xMutex_INVENTR);
		}
	}
	
	return ret;
}

//设置亮度级别，范围0~200
u8 InventrSetLightLevel(u8 level)
{
	u8 ret = 0;
	u8 i = 0;
	static u8 re_try_cnt = 0;
	u8 send_buf[8];
	
	if(InventrDisable == 0)
	{
		if(xSchedulerRunning == 1)
		{
			xSemaphoreTake(xMutex_INVENTR, portMAX_DELAY);
		}
		
		if(InventrBusy == 0)
		{
			InventrBusy = 1;
			
			memset(send_buf,0,8);
			
			send_buf[0] = 0x3A;
			send_buf[1] = 0x3C;
			send_buf[2] = 0x00;
			send_buf[3] = 0x01;
			send_buf[4] = level;
			send_buf[6] = 0x0D;
			send_buf[7] = 0x0A;
			
			for(i = 1; i <= 4; i ++)
			{
				send_buf[5] += send_buf[i];
			}
			
			RE_SEND:
			UsartSendString(UART4,send_buf,8);
			
			i = 10;
			
			while(i --)
			{
				delay_ms(100);
				
				if(Usart4RecvEnd == 0xAA)
				{
					Usart4RecvEnd = 0;
					
					if(MyStrstr(Usart4RxBuf, send_buf, Usart4FrameLen, 8) != 0xFFFF)
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
					}
					else
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
						
						i = 1;
						ret = 1;
					}
				}
			}
			
			if(ret == 0)
			{
				re_try_cnt ++;
				
				if(re_try_cnt < 10)
				{
					goto RE_SEND;
				}
			}
			
			InventrBusy = 0;
		}
		
		if(xSchedulerRunning == 1)
		{
			xSemaphoreGive(xMutex_INVENTR);
		}
	}
	
	return ret;
}

//读取输出电流
float InventrGetOutPutCurrent(void)
{
	static float current = 0.0f;
	u8 i = 0;
	u8 sum_cal = 0;
	u8 sum_recv = 0;
	u8 send_buf[8];
	u8 revc_buf[10];
//	u16 adc_val = 0;
	
	if(InventrDisable == 0)
	{
		if(xSchedulerRunning == 1)
		{
			xSemaphoreTake(xMutex_INVENTR, portMAX_DELAY);
		}
		
		if(InventrBusy == 0)
		{
			InventrBusy = 1;
			
			memset(send_buf,0,8);
			
			send_buf[0] = 0x3A;
			send_buf[1] = 0x3A;
			send_buf[2] = 0x00;
			send_buf[3] = 0x01;
			send_buf[4] = 0x02;
			send_buf[6] = 0x0D;
			send_buf[7] = 0x0A;
			
			for(i = 1; i <= 4; i ++)
			{
				send_buf[5] += send_buf[i];
			}
			
			UsartSendString(UART4,send_buf,8);
			
			i = 10;
			
			while(i --)
			{
				delay_ms(100);
				
				if(Usart4RecvEnd == 0xAA)
				{
					Usart4RecvEnd = 0;
					
					if(MyStrstr(Usart4RxBuf, send_buf, Usart4FrameLen, 8) != 0xFFFF)
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
					}
					else
					{
						memset(revc_buf,0,10);
						
						if(Usart4FrameLen <= 10)
						{
							memcpy(revc_buf,Usart4RxBuf,Usart4FrameLen);
							
							
							for(i = 1; i < Usart4FrameLen - 3; i ++)
							{
								sum_cal += revc_buf[i];
							}
							
							sum_recv = revc_buf[Usart4FrameLen - 3];
							
							if(sum_cal == sum_recv)
							{
								current = (float)((((u16)revc_buf[4]) << 8) + (u16)revc_buf[5]);			//新的通讯协议，读出来的值无需转换，直接是电流值ma
								
	//							if(adc_val > INVENTR_MAX_CURRENT_ADC_VAL)
	//							{
	//								adc_val = INVENTR_MAX_CURRENT_ADC_VAL;
	//							}
	//							
	//							current = (float)INVENTR_MAX_CURRENT_MA * ((float)adc_val / (float)INVENTR_MAX_CURRENT_ADC_VAL);
							}
						}
						
						memset(Usart4RxBuf,0,Usart4FrameLen);
						
						i = 1;
					}
				}
			}
			
			InventrBusy = 0;
		}
		
		if(xSchedulerRunning == 1)
		{
			xSemaphoreGive(xMutex_INVENTR);
		}
	}
	
	return current;
}

//读取输出电压
float InventrGetOutPutVoltage(void)
{
	static float voltage = 0.0f;
	u8 i = 0;
	u8 sum_cal = 0;
	u8 sum_recv = 0;
	u8 send_buf[8];
	u8 revc_buf[10];
//	u16 adc_val = 0;
	
	if(InventrDisable == 0)
	{
		if(xSchedulerRunning == 1)
		{
			xSemaphoreTake(xMutex_INVENTR, portMAX_DELAY);
		}
		
		if(InventrBusy == 0)
		{
			InventrBusy = 1;
			
			memset(send_buf,0,8);
			
			send_buf[0] = 0x3A;
			send_buf[1] = 0x3A;
			send_buf[2] = 0x01;
			send_buf[3] = 0x01;
			send_buf[4] = 0x02;
			send_buf[6] = 0x0D;
			send_buf[7] = 0x0A;
			
			for(i = 1; i <= 4; i ++)
			{
				send_buf[5] += send_buf[i];
			}
			
			UsartSendString(UART4,send_buf,8);
			
			i = 10;
			
			while(i --)
			{
				delay_ms(100);
				
				if(Usart4RecvEnd == 0xAA)
				{
					Usart4RecvEnd = 0;
					
					if(MyStrstr(Usart4RxBuf, send_buf, Usart4FrameLen, 8) != 0xFFFF)
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
					}
					else
					{
						memset(revc_buf,0,10);
						
						if(Usart4FrameLen <= 10)
						{
							memcpy(revc_buf,Usart4RxBuf,Usart4FrameLen);
							
							
							for(i = 1; i < Usart4FrameLen - 3; i ++)
							{
								sum_cal += revc_buf[i];
							}
							
							sum_recv = revc_buf[Usart4FrameLen - 3];
							
							if(sum_cal == sum_recv)
							{
								voltage = (float)((((u16)revc_buf[4]) << 8) + (u16)revc_buf[5]);				//新的通讯协议，读出来的值无需转换，直接是电压值V
								
	//							if(adc_val > INVENTR_MAX_VOLTAGE_ADC_VAL)
	//							{
	//								adc_val = INVENTR_MAX_VOLTAGE_ADC_VAL;
	//							}
	//							
	//							voltage = (float)INVENTR_MAX_VOLTAGE_V * ((float)adc_val / (float)INVENTR_MAX_VOLTAGE_ADC_VAL);
							}
						}
						
						memset(Usart4RxBuf,0,Usart4FrameLen);
						
						i = 1;
					}
				}
			}
			
			InventrBusy = 0;
		}
		
		if(xSchedulerRunning == 1)
		{
			xSemaphoreGive(xMutex_INVENTR);
		}
	}
	
	return voltage;
}

//读取机种信息
u8 InventrGetDeviceInfo(void)
{
	u8 ret = 0;
	u8 i = 0;
	u8 send_buf[8];
	
	if(InventrDisable == 0)
	{
		if(xSchedulerRunning == 1)
		{
			xSemaphoreTake(xMutex_INVENTR, portMAX_DELAY);
		}
		
		if(InventrBusy == 0)
		{
			InventrBusy = 1;
			
			memset(send_buf,0,8);
			
			send_buf[0] = 0x3A;
			send_buf[1] = 0x35;
			send_buf[2] = 0x0B;
			send_buf[3] = 0x01;
			send_buf[4] = 0x05;
			send_buf[6] = 0x0D;
			send_buf[7] = 0x0A;
			
			for(i = 1; i <= 4; i ++)
			{
				send_buf[5] += send_buf[i];
			}
			
			UsartSendString(UART4,send_buf,8);
			
			i = 10;
			
			while(i --)
			{
				delay_ms(100);
				
				if(Usart4RecvEnd == 0xAA)
				{
					Usart4RecvEnd = 0;
					
					if(MyStrstr(Usart4RxBuf, send_buf, Usart4FrameLen, 8) != 0xFFFF)
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
					}
					else
					{
						memset(Usart4RxBuf,0,Usart4FrameLen);
						
						i = 1;
						ret = 1;
					}
				}
			}
			
			InventrBusy = 0;
		}
		
		if(xSchedulerRunning == 1)
		{
			xSemaphoreGive(xMutex_INVENTR);
		}
	}
	
	return ret;
}


void SetLightLevel(u8 intfc_type, u8 level)
{
	if(intfc_type > 2 || level > 200)
	{
		return;
	}
	
	switch(intfc_type)
	{
		case 0:
			Mcp4725SetOutPutVoltage(((float)level / 200.0f) * 5.0f);
		break;
		
		case 1:
			PWMSetLightLevel(level * 10);
		break;
		
		case 2:
			InventrSetLightLevel(level);
		break;
		
		default:
			
		break;
	}
}









