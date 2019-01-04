#ifndef __IIC1_H
#define __IIC1_H
#include "sys.h"
#include "common.h"


#define IIC1_PORT          		GPIOB
#define IIC1_SCL_PIN           	GPIO_Pin_13
#define IIC1_SDA_PIN           	GPIO_Pin_14

#define IIC1_SCL_H            	GPIO_SetBits(IIC1_PORT,IIC1_SCL_PIN)
#define IIC1_SCL_L       		GPIO_ResetBits(IIC1_PORT,IIC1_SCL_PIN)

#define IIC1_SDA_H     			GPIO_SetBits(IIC1_PORT,IIC1_SDA_PIN)
#define IIC1_SDA_L        		GPIO_ResetBits(IIC1_PORT,IIC1_SDA_PIN)

#define IIC1_SCL_IN            	GPIO_ReadInputDataBit(IIC1_PORT,IIC1_SDA_PIN)
#define IIC1_SDA_IN  			GPIO_ReadInputDataBit(IIC1_PORT,IIC1_SDA_PIN)

#define IIC1_SET_SDA_IN()   	{IIC1_PORT->CRH &= 0XF0FFFFFF; IIC1_PORT->CRH |= (u32)8 << (u32)24;}
#define IIC1_SET_SDA_OUT()  	{IIC1_PORT->CRH &= 0XF0FFFFFF; IIC1_PORT->CRH |= (u32)3 << (u32)24;}



//IIC所有操作函数
void IIC1_Init(void);                	//初始化IIC的IO口				 
void IIC1_Start(void);					//发送IIC开始信号
void IIC1_Stop(void);	  				//发送IIC停止信号
void IIC1_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC1_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 IIC1_Wait_Ack(void); 				//IIC等待ACK信号
void IIC1_Ack(void);					//IIC发送ACK信号
void IIC1_NAck(void);					//IIC不发送ACK信号

void IIC1_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC1_Read_One_Byte(u8 daddr,u8 addr);

#endif
















