#ifndef __TASK_SENSOR_H
#define __TASK_SENSOR_H

#include "sys.h"
#include "rtos_task.h"


typedef struct SensorMsg
{
	u16 temperature;
	u16 humidity;
	u16 illumination;
	u16 in_put_current;
	u16 in_put_voltage;
	u16 in_put_freq;
	s16 in_put_power_p;
	s16 in_put_power_q;
	u16 in_put_power_s;
	u32 in_put_energy_p;
	u32 in_put_energy_q;
	u32 in_put_energy_s;
	u16 out_put_current;
	u16 out_put_voltage;
	u8 signal_intensity;
	u8 hour;
	u8 minute;
	u8 second;
	char gps[32];
}SensorMsg_S;


extern TaskHandle_t xHandleTaskSENSOR;

void vTaskSENSOR(void *pvParameters);






































#endif
