#include "cd4051b.h"


void CD4051B_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void cd4051b_set_channel(u8 ch)
{
	if(ch > 2)
	{
		return;
	}
	
	switch(ch)
	{
		case 0:
			CD4051B_CS2_L;
			CD4051B_CS1_L;
		break;
		
		case 1:
			CD4051B_CS2_L;
			CD4051B_CS1_H;
		break;
		
		case 2:
			CD4051B_CS2_H;
			CD4051B_CS1_L;
		break;
		
		default:
		break;
	}
}



















