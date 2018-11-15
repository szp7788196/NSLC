#include "delay.h"
#include "sys.h"
#include "common.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#include "task.h"
#endif

static u32 fac_us=0;

#if SYSTEM_SUPPORT_OS
    static u16 fac_ms = 0;
#endif


#if SYSTEM_SUPPORT_OS
void delay_osschedlock(void)
{
#ifdef 	SYSTEM_SUPPORT_OS
	vTaskSuspendAll();
#endif
}

void delay_osschedunlock(void)
{
#ifdef 	SYSTEM_SUPPORT_OS
	xTaskResumeAll();
#endif
}

void delay_ostimedly(u32 ticks)
{
#ifdef 	SYSTEM_SUPPORT_OS
	vTaskDelay(ticks);
#endif
}

void SysTick_Handler(void)
{
	SysTick1msAdder();			//1msµÎ´ð¼ÆÊýÆ÷ÀÛ¼Ó

	if(xSchedulerRunning == 1)
	{
		xPortSysTickHandler();
	}
}
#endif


void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS
	u32 reload;
#endif

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us = SYSCLK;

#if SYSTEM_SUPPORT_OS
	reload = SYSCLK;
	reload *= 1000000 / configTICK_RATE_HZ;

	fac_ms = 1000 / configTICK_RATE_HZ;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD = reload;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#else

#endif
}

#if SYSTEM_SUPPORT_OS

//nus:0~190887435
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;

	ticks = nus * fac_us;
	delay_osschedlock();
	told = SysTick->VAL;

	while(1)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
				tcnt += told-tnow;
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if(tcnt >= ticks)
				break;
		}
	};

	delay_osschedunlock();
}

//nms:0~65535
void delay_ms(u16 nms)
{
	if(xSchedulerRunning)
	{
		if(nms >= fac_ms)
		{
   			delay_ostimedly(nms / fac_ms);
		}
		nms %= fac_ms;
	}

	delay_us((u32)(nms * 1000));
}
#else

//nus:0~190887435
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick -> LOAD;

	ticks = nus * fac_us;
	told = SysTick->VAL;

	while(1)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
				tcnt += told - tnow;
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if(tcnt >= ticks)
				break;
		}
	};
}

void delay_ms(u16 nms)
{
	u32 i;
	for(i = 0; i < nms; i++)
		delay_us(1000);
}
#endif





































