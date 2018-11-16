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

float current = 0;
float voltage = 0;
float freq = 0.0f;
float power_p = 0.0f;
float power_q = 0.0f;
float power_s = 0.0f;
float energy_p = 0.0f;
float energy_q = 0.0f;
float energy_s = 0.0f;

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

		current = Att7059xGetCurrent1();
		delay_ms(300);
		voltage = Att7059xGetVoltage();
		delay_ms(300);
		freq = Att7059xGetVoltageFreq();
		delay_ms(300);
		power_p = Att7059xGetChannel1PowerP();
		delay_ms(300);
		power_q = Att7059xGetChannel1PowerQ();
		delay_ms(300);
		power_s = Att7059xGetChannel1PowerS();
		delay_ms(300);
		energy_p = Att7059xGetEnergyP();
		delay_ms(300);
		energy_q = Att7059xGetEnergyQ();
		delay_ms(300);
		energy_s = Att7059xGetEnergyS();
		delay_ms(300);

		if(push_data_to_net == 1)
		{
			push_data_to_net = 0;

			if(ConnectState == ON_SERVER)					//设备此时是在线状态
			{
#ifndef	SMALLER_BOARD
				p_tSensorMsg->temperature = (u16)(Temperature * 10.0f + 0.5f);
				p_tSensorMsg->humidity = (u16)(Humidity * 10.0f + 0.5f);
				p_tSensorMsg->illumination = (u16)(Illumination + 0.5f);
#endif
				p_tSensorMsg->out_put_current = (u16)(InventrOutPutCurrent + 0.5f);
				p_tSensorMsg->out_put_voltage = (u16)(InventrOutPutVoltage + 0.5f);
				p_tSensorMsg->signal_intensity = 113 - (SignalIntensity * 2);
				p_tSensorMsg->hour = calendar.hour;
				p_tSensorMsg->minute = calendar.min;
				p_tSensorMsg->second = calendar.sec;

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






































