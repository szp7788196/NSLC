#include "timer.h"
#include "usart.h"
#include "dali.h"



void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM2,TIM_IT_Update ,ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
	static u8 tick = 0;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		if(tick < 96)
		{
			tick ++;

			if(tick >= 96)					//约10ms
			{
				tick = 0;

				Usart1ReciveFrameEnd();		//检测USART1是否接收完成一帧数据
				Usart3ReciveFrameEnd();		//检测USART3是否接收完成一帧数据
				Usart4ReciveFrameEnd();		//检测UART4是否接收完成一帧数据
				Usart5ReciveFrameEnd();		//检测UART5是否接收完成一帧数据
			}
		}

		if(dali_get_flag()==RECEIVING_DATA)
		{
			dali_receive_tick();
		}
		else if(dali_get_flag()==SENDING_DATA)
		{
			dali_send_tick();
		}
	}
}






