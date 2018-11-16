#include "task_sensor.h"
#include "delay.h"
#include "sht2x.h"
#include "bh1750.h"
#include "task_net.h"
#include "common.h"
#include "inventr.h"
#include "rtc.h"
#include "usart.h"
#include "att7059x.h"

float InputCurrent = 0;
float InputVoltage = 0;
float InputFreq = 0.0f;
float InputPowerP = 0.0f;
float InputPowerQ = 0.0f;
float InputPowerS = 0.0f;
float InputEnergyP = 0.0f;
float InputEnergyQ = 0.0f;
float InputEnergyS = 0.0f;

TaskHandle_t xHandleTaskSENSOR = NULL;

SensorMsg_S *p_tSensorMsg = NULL;	//用于装在传感器数据的结构体变量
unsigned portBASE_TYPE SENSOR_Satck;
void vTaskSENSOR(void *pvParameters)
{
	time_t times_sec_up = 0;
	time_t times_sec_sim = 0;
//	u32 cnt = 0;
	u8 push_data_to_net = 0;

#ifndef SMALLER_BOARD
	SHT2x_Init();
	Bh1750_Init();
#endif

	p_tSensorMsg = (SensorMsg_S *)mymalloc(sizeof(SensorMsg_S));

	while(1)
	{
		if(GetSysTick1s() - times_sec_up >= UpLoadINCL)		//每隔UpLoadINCL秒向网络任务推送一次传感器数据
		{
			times_sec_up = GetSysTick1s();
			push_data_to_net = 1;
		}

#ifndef SMALLER_BOARD
		Temperature = Sht2xReadTemperature();				//读取温度
		Humidity = Sht2xReadHumidity();						//读取湿度
		Illumination = Bh1750ReadIllumination();			//读取光照
#endif

		if(GetSysTick1s() - times_sec_sim >= UpLoadINCL)		//每隔UpLoadINCL秒向网络任务推送一次传感器数据
		{
			times_sec_sim = GetSysTick1s();

			InventrOutPutCurrent = InventrGetOutPutCurrent();	//读取电源输出电流
			delay_ms(500);
			InventrOutPutVoltage = InventrGetOutPutVoltage();	//读取电源输出电压
		}

		InputCurrent = Att7059xGetCurrent1();
		delay_ms(300);
		InputVoltage = Att7059xGetVoltage();
		delay_ms(300);
		InputFreq = Att7059xGetVoltageFreq();
		delay_ms(300);
		InputPowerP = Att7059xGetChannel1PowerP();
		delay_ms(300);
		InputPowerQ = Att7059xGetChannel1PowerQ();
		delay_ms(300);
		InputPowerS = Att7059xGetChannel1PowerS();
		delay_ms(300);
		InputEnergyP = Att7059xGetEnergyP();
		delay_ms(300);
		InputEnergyQ = Att7059xGetEnergyQ();
		delay_ms(300);
		InputEnergyS = Att7059xGetEnergyS();
		delay_ms(300);

		if(push_data_to_net == 1)
		{
			push_data_to_net = 0;

			if(ConnectState == ON_SERVER)					//设备此时是在线状态
			{
#ifndef	SMALLER_BOARD
				p_tSensorMsg->temperature 		= (u16)(Temperature * 10.0f + 0.5f);
				p_tSensorMsg->humidity 			= (u16)(Humidity * 10.0f + 0.5f);
				p_tSensorMsg->illumination 		= (u16)(Illumination + 0.5f);
#endif
				p_tSensorMsg->in_put_current 	= (u16)(InputCurrent + 0.5f);
				p_tSensorMsg->in_put_voltage 	= (u16)(InputVoltage + 0.5f);
				p_tSensorMsg->in_put_freq 		= (u16)(InputFreq * 100.0f + 0.5f);
				p_tSensorMsg->in_put_power_p 	= (u16)(InputPowerP + 0.5f);
				p_tSensorMsg->in_put_power_q 	= (u16)(InputPowerQ + 0.5f);
				p_tSensorMsg->in_put_power_s 	= (u16)(InputPowerS + 0.5f);
				p_tSensorMsg->in_put_energy_p 	= (u32)(InputEnergyP * 1000.0f + 0.5f);
				p_tSensorMsg->in_put_energy_q 	= (u32)(InputEnergyQ * 1000.0f + 0.5f);
				p_tSensorMsg->in_put_energy_s 	= (u32)(InputEnergyS * 1000.0f + 0.5f);

				p_tSensorMsg->out_put_current 	= (u16)(InventrOutPutCurrent + 0.5f);
				p_tSensorMsg->out_put_voltage 	= (u16)(InventrOutPutVoltage + 0.5f);
				p_tSensorMsg->signal_intensity 	= 113 - (SignalIntensity * 2);
				p_tSensorMsg->hour 				= calendar.hour;
				p_tSensorMsg->minute 			= calendar.min;
				p_tSensorMsg->second 			= calendar.sec;

				memset(p_tSensorMsg->gps,0,32);

				if(GpsInfo != NULL && strlen((char *)GpsInfo) <= 32)
				{
					memcpy(p_tSensorMsg->gps,GpsInfo,strlen((char *)GpsInfo));
				}
				else
				{
					memcpy(p_tSensorMsg->gps,"3948.0975N11632.7539E",21);
				}

				if(xQueueSend(xQueue_sensor,(void *)p_tSensorMsg,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					UsartSendString(USART1,"send p_tSensorMsg fail 1.\r\n",27);
#endif
				}
			}
		}

//		cnt = (cnt + 1) & 0xFFFFFFFF;
//		delay_ms(100);

		SENSOR_Satck = uxTaskGetStackHighWaterMark(NULL);
	}
}






































