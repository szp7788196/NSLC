#ifndef __IIC2_H
#define __IIC2_H
#include "sys.h"  


//#define IIC2_PORT          		GPIOB
#define IIC2_SCL_PIN           	GPIO_Pin_8
#define IIC2_SDA_PIN           	GPIO_Pin_9

#define IIC2_SCL_H            	GPIO_SetBits(GPIOA,IIC2_SCL_PIN)
#define IIC2_SCL_L       		GPIO_ResetBits(GPIOA,IIC2_SCL_PIN)

#define IIC2_SDA_H     			GPIO_SetBits(GPIOC,IIC2_SDA_PIN)
#define IIC2_SDA_L        		GPIO_ResetBits(GPIOC,IIC2_SDA_PIN)

#define IIC2_SCL_IN            	GPIO_ReadInputDataBit(GPIOA,IIC2_SDA_PIN)
#define IIC2_SDA_IN  			GPIO_ReadInputDataBit(GPIOC,IIC2_SDA_PIN)

#define IIC2_SET_SDA_IN()   	{GPIOC->CRH &= 0XFFFFFF0F; GPIOC->CRH |= (u32)8 << (u32)4;}
#define IIC2_SET_SDA_OUT()  	{GPIOC->CRH &= 0XFFFFFF0F; GPIOC->CRH |= (u32)3 << (u32)4;}


//IIC所有操作函数
void IIC2_Init(void);                	//初始化IIC的IO口				 
void IIC2_Start(void);					//发送IIC开始信号
void IIC2_Stop(void);	  				//发送IIC停止信号
void IIC2_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC2_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 IIC2_Wait_Ack(void); 				//IIC等待ACK信号
void IIC2_Ack(void);					//IIC发送ACK信号
void IIC2_NAck(void);					//IIC不发送ACK信号

void IIC2_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC2_Read_One_Byte(u8 daddr,u8 addr);

#endif
















