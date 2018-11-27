#include "dali.h"
#include "string.h"



u8 flag;        		// status flag
u8 bit_count;   		// nr of rec/send bits
u16 tick_count; 		// nr of ticks of the timer

bool bit_value;   		// value of actual bit
bool actual_val;  		// bit value in this tick of timer
bool former_val;  		// bit value in previous tick of timer

u8 recv_num;
u16 send_num;
bool recv_ok;


void DALI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;				//DALI TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;				//DALI RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line15);
}

u8 dali_get_flag(void)
{
	return flag;
}

void dali_receive_data(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	bit_count = 0;
	tick_count = 0;
	former_val = TRUE;
	flag = RECEIVING_DATA;
	recv_num = 0;
	recv_ok = FALSE;

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void dali_send_data(u8 address, u8 command)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	send_num = ((((u16)address) << 8) & 0xFF00) + ((u16)command & 0x00FF);
	bit_count = 0;
	tick_count = 0;

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);

	flag = SENDING_DATA;
}

bool dali_get_DALIIN(void)
{
	if (DALIIN_INVERT)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15))
			return TRUE;
		else
			return FALSE;
	}
}

void dali_set_DALIOUT(bool pin_value)
{
	if (DALIOUT_INVERT)
	{
		if(pin_value)
			GPIOC->ODR &= ~GPIO_Pin_6;
		else
			GPIOC->ODR |= GPIO_Pin_6;
	}
	else
	{
		if(pin_value)
			GPIOC->ODR |= GPIO_Pin_6;
		else
			GPIOC->ODR &= ~GPIO_Pin_6;
	}
}

bool dali_get_DALIOUT(void)
{
	if (DALIOUT_INVERT)
	{
		if(GPIOC->IDR & GPIO_Pin_6)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GPIOC->IDR & GPIO_Pin_6)
			return TRUE;
		else
			return FALSE;
	}
}

void dali_receive_tick(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	// Because of the structure of current amplifier, input has to be negated
	actual_val = dali_get_DALIIN();
	tick_count ++;

	if(actual_val != former_val)					// edge detected
	{
		switch(bit_count)
		{
			case 0:
				if (tick_count > 2)
				{
					tick_count = 0;
					bit_count  = 1; 				// start bit
				}
			break;

			case 17:      							// 1st stop bit
				if(tick_count > 6) 					// stop bit error, no edge should exist
					flag = ERR;
			break;

			default:      							// other bits
			if(tick_count > 6)
			{
				if(bit_count < 9) 					// store bit in address byte
				{
					recv_num |= (actual_val << (8 - bit_count));
				}
//				else             					// store bit in data byte(主机模式下只接收一个字节，此处屏蔽掉)
//				{
//					dataByte |= (actual_val << (16-bit_count));
//				}

				bit_count ++;
				tick_count = 0;
			}
			break;
		}
	}
	else 											// voltage level stable
	{
		switch(bit_count)
		{
			case 0:
				if(tick_count == 8)  				// too long start bit
					flag = ERR;
			break;

			case 9:									// First stop bit
			if (tick_count == 8)
			{
				if (actual_val == 0) 				// wrong level of stop bit
				{
					flag = ERR;
				}
				else
				{
					bit_count ++;
					tick_count = 0;
				}
			}
			break;

			case 10:								// Second stop bit
			if (tick_count == 8)
			{
				flag = NO_ACTION;

				EXTI_InitStructure.EXTI_Line = EXTI_Line15;
				EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
				EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
				EXTI_InitStructure.EXTI_LineCmd = ENABLE;
				EXTI_Init(&EXTI_InitStructure);

				recv_ok = TRUE;
			}
			break;

			default: 								// normal bits
			if(tick_count == 10)					// too long delay before edge
			{
				flag = ERR;
			}
			break;
		}
	}

	former_val = actual_val;

	if(flag == ERR)
	{
		flag = NO_ACTION;

		EXTI_InitStructure.EXTI_Line = EXTI_Line15;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
}

void dali_send_tick(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	if((tick_count & 0x03) == 0)						//access to the routine just every 4 ticks = every half bit
	{
		if(tick_count < 160)
		{

			if(tick_count < 24)							// settling time between forward and backward frame
			{
				tick_count ++;
				return;
			}


			if(tick_count == 24)						// start of the start bit
			{
				dali_set_DALIOUT(FALSE);
				tick_count ++;
				return;
			}
														// edge of the start bit
			if(tick_count == 28)						// 28 ticks = 28/9600 = 2,92ms = delay between forward and backward message frame
			{
				dali_set_DALIOUT(TRUE);
				tick_count ++;
				return;
			}

														// bit value (edge) selection
			bit_value = (bool)((send_num >> (15 - bit_count)) & 0x01);


			if(!((tick_count - 24) & 0x0007))			// Every half bit -> Manchester coding
			{ 											// div by 8
				if(dali_get_DALIOUT() == bit_value) 	// former value of bit = new value of bit
					dali_set_DALIOUT((bool)(1 - bit_value));
			}

			if(!((tick_count - 28) & 0x0007))			// Generate edge for actual bit
			{
				dali_set_DALIOUT(bit_value);
				bit_count ++;
			}
		}
		else
		{
			if(tick_count == 160)						// end of data byte, start of stop bits
			{
				dali_set_DALIOUT(TRUE); 				// start of stop bit
			}

			if(tick_count == 176)						// end of stop bits, no settling time
			{
				flag = NO_ACTION;

				EXTI_InitStructure.EXTI_Line = EXTI_Line15;
				EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
				EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
				EXTI_InitStructure.EXTI_LineCmd = ENABLE;
				EXTI_Init(&EXTI_InitStructure);
			}
		}
	}

	tick_count ++;

	return;
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		dali_receive_data();

		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}













