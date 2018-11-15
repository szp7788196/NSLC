#include "task_led.h"
#include "led.h"
#include "delay.h"
#include "task_net.h"


TaskHandle_t xHandleTaskLED = NULL;
unsigned portBASE_TYPE LED_Satck;
void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 led_state = 0;
	
	while(1)
	{
		if(cnt % 50 == 0)					//每隔0.5秒喂看门狗
		{
			IWDG_Feed();
		}
		
		if(ConnectState == ON_SERVER)		//在线状态，每3秒短闪一次
		{
			if(cnt % 300 == 0)
			{
				led_state = 1;
			}
			else
			{
				led_state = 0;
			}
		}
		else								//离线状态，连续闪烁，周期600ms
		{
			if(cnt % 30 == 0)
			{
				led_state = ~led_state;
			}
		}
		
		if(led_state)
		{
			RUN_LED = 0;
		}
		else
		{
			RUN_LED = 1;
		}
		
		cnt = (cnt + 1) & 0xFFFFFFFF;
		
		delay_ms(10);
		
		LED_Satck = uxTaskGetStackHighWaterMark(NULL);
	}
}






































