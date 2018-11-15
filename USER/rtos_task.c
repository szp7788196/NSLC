#include "rtos_task.h"
#include "common.h"
#include "task_led.h"
#include "task_hci.h"
#include "task_sensor.h"
#include "task_net.h"
#include "task_main.h"

/*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
void AppObjCreate(void)
{
	//创建互斥量
	xMutex_IIC1 = xSemaphoreCreateMutex();
	if(xMutex_IIC1 == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
	xMutex_INVENTR = xSemaphoreCreateMutex();
	if(xMutex_INVENTR == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
	xMutex_AT_COMMAND = xSemaphoreCreateMutex();
	if(xMutex_AT_COMMAND == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
	
	//创建消息队列
	xQueue_sensor = xQueueCreate(10, sizeof(SensorMsg_S));
    if( xQueue_sensor == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
}

/*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************/
void AppTaskCreate(void)
{
	xTaskCreate(vTaskLED,
				"vTaskLED",
				64,
				NULL,
				3,
				&xHandleTaskLED);		//指示灯任务
	
	xTaskCreate(vTaskHCI,
				"vTaskHCI",
				512,
				NULL,
				4,
				&xHandleTaskHCI);		//人机交互任务
	
	xTaskCreate(vTaskSENSOR,
				"vTaskSENSOR",
				128,
				NULL,
				5,
				&xHandleTaskSENSOR);	//传感器采集任务
	
	xTaskCreate(vTaskNET,
				"vTaskNET",
				2048,
				NULL,
				6,
				&xHandleTaskNET);		//网络任务
	
	xTaskCreate(vTaskMAIN,
				"vTaskMAIN",
				128,
				NULL,
				7,
				&xHandleTaskMAIN);		//主任务
}

























































