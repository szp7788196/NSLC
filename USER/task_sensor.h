#ifndef __TASK_SENSOR_H
#define __TASK_SENSOR_H

#include "sys.h"
#include "rtos_task.h"


typedef struct SensorMsg
{
	u16 temperature;
	u16 humidity;
	u16 illumination;
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
