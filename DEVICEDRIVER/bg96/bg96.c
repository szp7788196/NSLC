#include "bg96.h"
#include "usart.h"
#include "common.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>

pBg96 bg96;

unsigned char net_temp_data_rx_buf[8];

unsigned char BG96_InitStep1(pBg96 *bg96)
{
	unsigned char ret = 0;

	(*bg96) = (pBg96)mymalloc(sizeof(Bg96));
	if((*bg96) == NET_NULL)
	{
		return 0;
	}

	(*bg96)->init_ok = 0;

	(*bg96)->USARTx = USART2;

	(*bg96)->rx_cmd_buf = (char *)mymalloc(sizeof(char) * CMD_DATA_BUFFER_SIZE);
	if((*bg96)->rx_cmd_buf == NET_NULL)
	{
		return 0;
	}

	ret = RingBuf_Init(&(*bg96)->net_buf, NET_DATA_BUFFER_SIZE);
	if(ret == 0)
	{
		return ret;
	}

	(*bg96)->net_data_rx_cnt = 0;
	(*bg96)->net_data_len = 0;
	(*bg96)->net_data_id = 0;
	(*bg96)->break_out_wait_cmd = 0;
	(*bg96)->rx_cnt = 0;
	(*bg96)->imei = NULL;

	(*bg96)->bg96_mode 						= NET_MODE;
	(*bg96)->cip_mux_mode					= SIGNLE;
	(*bg96)->last_time 						= GetSysTick1ms();

	(*bg96)->hard_init						= bg96_hard_init;
	(*bg96)->hard_enable 					= bg96_hard_enable;
	(*bg96)->hard_disable 					= bg96_hard_disable;
	(*bg96)->hard_reset 					= bg96_hard_reset;
	(*bg96)->send_string 					= bg96_send_string;

	(*bg96)->available 						= bg96_available;
	(*bg96)->read 							= bg96_read;
	(*bg96)->send 							= bg96_send;

	(*bg96)->restart 						= bg96_restart;
	(*bg96)->get_connect_state 				= bg96_get_connect_state;
	(*bg96)->get_local_ip 					= bg96_get_local_ip;

	(*bg96)->create_TCP 					= bg96_create_TCP;
	(*bg96)->release_TCP 					= bg96_release_TCP;
	(*bg96)->register_UDP 					= bg96_register_UDP;
	(*bg96)->unregister_UDP 				= bg96_unregister_UDP;

	(*bg96)->set_AT 						= bg96_set_AT;
	(*bg96)->set_AT_ATE 					= bg96_set_AT_ATE;
	(*bg96)->set_AT_UART 					= bg96_set_AT_UART;
	(*bg96)->get_AT_CPIN 					= bg96_get_AT_CPIN;
	(*bg96)->get_AT_CSQ 					= bg96_get_AT_CSQ;

    (*bg96)->set_AT_QCFG1 					= bg96_set_AT_QCFG1;
    (*bg96)->set_AT_QCFG2 					= bg96_set_AT_QCFG2;
    (*bg96)->set_AT_QCFG3 					= bg96_set_AT_QCFG3;
    (*bg96)->set_AT_QCFG4 					= bg96_set_AT_QCFG4;
    (*bg96)->set_AT_QCFG5 					= bg96_set_AT_QCFG5;
	(*bg96)->set_AT_QICSGP 					= bg96_set_AT_QICSGP;
    (*bg96)->set_AT_QIACT 					= bg96_set_AT_QIACT;
	(*bg96)->get_AT_QIACT 					= bg96_get_AT_QIACT;
    (*bg96)->get_AT_QISTATE 				= bg96_get_AT_QISTATE;

    (*bg96)->set_AT_QIOPEN 					= bg96_set_AT_QIOPEN;
    (*bg96)->set_AT_QICLOSE 				= bg96_set_AT_QICLOSE;
    (*bg96)->set_AT_QISEND 					= bg96_set_AT_QISEND;
	(*bg96)->get_AT_QISEND					= bg96_get_AT_QISEND;
	(*bg96)->get_AT_QIDNSGIP 				= bg96_get_AT_QIDNSGIP;
    (*bg96)->get_AT_QPING 					= bg96_get_AT_QPING;
	(*bg96)->get_AT_GSN 					= bg96_get_AT_GSN;

	(*bg96)->set_AT_QGPS					= bg96_set_AT_QGPS;
	(*bg96)->set_AT_QGPSLOC 				= bg96_set_AT_QGPSLOC;
	(*bg96)->set_AT_QGPSEND					= bg96_set_AT_QGPSEND;

	(*bg96)->set_AT_QNTP					= bg96_set_AT_QNTP;

    (*bg96)->clear_rx_cmd_buffer 			= bg96_clear_rx_cmd_buffer;
    (*bg96)->get_char 						= bg96_get_char;

    (*bg96)->print_rx_buf 					= bg96_print_rx_buf;
    (*bg96)->print_cmd 						= bg96_print_cmd;
    (*bg96)->wait_cmd1 						= bg96_wait_cmd1;
    (*bg96)->wait_cmd2 						= bg96_wait_cmd2;
    (*bg96)->wait_bg96_mode 				= bg96_wait_bg96_mode;

	(*bg96)->uart_interrupt_event 			= bg96_uart_interrupt_event;
    (*bg96)->net_data_state_process 		= bg96_net_data_state_process;

	(*bg96)->init_ok = 1;

	return ret;
}


void BG96_UnInit(pBg96 *bg96)
{
	myfree((*bg96)->net_buf->buf);
	myfree(((*bg96)->net_buf));
	myfree((*bg96)->rx_cmd_buf);
	myfree(*bg96);
}

u8 signal = 0;
unsigned char BG96_InitStep2(pBg96 *bg96)
{
	static u8 hard_inited = 0;
	u8 ret = 0;
	u8 fail_time = 0;
	u8 qiact_fail_times = 0;
	u8 buf[64];

	if(hard_inited == 0)
	{
		(*bg96)->hard_init(bg96);
	}

	RE_HARD_RESET:

	(*bg96)->hard_reset(bg96);
	delay_ms(10000);

	(*bg96)->clear_rx_cmd_buffer(bg96);
	(*bg96)->net_buf->clear(&(*bg96)->net_buf);
	(*bg96)->net_data_rx_cnt = 0;
	(*bg96)->net_data_len = 0;
	(*bg96)->net_data_id = 0;
	(*bg96)->break_out_wait_cmd = 0;
	(*bg96)->rx_cnt = 0;
	(*bg96)->bg96_mode = NET_MODE;
	(*bg96)->cip_mux_mode = SIGNLE;
	(*bg96)->last_time = GetSysTick1ms();

//	(*bg96)->set_AT_UART(bg96,115200);

	fail_time = 0;
	while(!(*bg96)->set_AT(bg96))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_ATE(bg96, 0))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->get_AT_CPIN(bg96))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

//	fail_time = 0;
//	while(!(*bg96)->set_AT_QCFG1(bg96))
//	{
//		fail_time ++;
//		if(fail_time >= 3)
//		{
//			goto RE_HARD_RESET;
//		}
//	}
//	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_QCFG2(bg96))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_QCFG3(bg96))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_QCFG4(bg96,Operators))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_QCFG5(bg96))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

//	fail_time = 0;
//	while((*bg96)->get_AT_CSQ(bg96))
//	{
//		fail_time ++;
//		if(fail_time >= 1)
//		{
//			goto RE_HARD_RESET;
//		}
//	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->set_AT_QICSGP(bg96,Operators))
	{
		fail_time ++;
		if(fail_time >= 3)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	if(qiact_fail_times >= 3)
	{
		qiact_fail_times = 0;

		delay_ms(20000);
	}

	fail_time = 0;
	while(!(*bg96)->set_AT_QIACT(bg96))
	{
		fail_time ++;
		if(fail_time >= 1)
		{
			signal = (*bg96)->get_AT_CSQ(bg96);
			qiact_fail_times ++;
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->get_AT_QIACT(bg96,(char *)buf))
	{
		fail_time ++;
		if(fail_time >= 1)
		{
			goto RE_HARD_RESET;
		}
	}
	delay_ms(100);

	fail_time = 0;
	while(!(*bg96)->get_AT_GSN(bg96))
	{
		fail_time ++;
		if(fail_time >= 1)
		{
			goto RE_HARD_RESET;
		}
	}

	delay_ms(500);

	return ret;
}

void bg96_hard_init(pBg96 *bg96)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	BG96_PWREN_LOW;
	BG96_PWRKEY_LOW;
	BG96_RST_LOW;
}


void bg96_hard_enable(pBg96 *bg96)
{
	BG96_PWRKEY_HIGH;
	delay_ms(200);
	BG96_PWRKEY_LOW;
}


void bg96_hard_disable(pBg96 *bg96)
{
	BG96_PWRKEY_HIGH;
	delay_ms(1000);
	BG96_PWRKEY_LOW;
}


void bg96_hard_reset(pBg96 *bg96)
{
	u8 i = 0;

	RE_START:
	BG96_PWREN_LOW;						//关闭电源
	delay_ms(300);
	BG96_PWREN_HIGH;					//打开电源

	delay_ms(100);

	BG96_RST_HIGH;						//硬件复位
	delay_ms(300);
	BG96_RST_LOW;

	delay_ms(100);

	if(READ_BG96_STATUS == 1)			//关机状态
	{
		(*bg96)->hard_enable(bg96);		//发送开机脉冲

		while(READ_BG96_STATUS == 1)	//等待检测是否已经开机
		{
			delay_ms(100);

			if((i ++) > 100)
			{
				i = 0;
				break;
			}
		}

		return;
	}
	else if(READ_BG96_STATUS == 0)		//开机状态
	{
		(*bg96)->hard_disable(bg96);	//发送关机脉冲

		while(READ_BG96_STATUS == 0)	//等待检测是否已经关机
		{
			delay_ms(100);

			if((i ++) > 100)
			{
				i = 0;
				break;
			}
		}

		if(READ_BG96_STATUS == 1)
		{
			delay_ms(1000);

			goto RE_START;
		}
	}
}

unsigned char bg96_send_string(pBg96 *bg96,USART_TypeDef* USARTx,unsigned char *str, unsigned short len)
{
	if(USARTx == USART1)
	{
		memcpy(Usart1TxBuf,str,len);
		Usart1SendLen = len;
	}
	else if(USARTx == USART2)
	{
		memcpy(Usart2TxBuf,str,len);
		Usart2SendLen = len;
	}
	else if(USARTx == UART4)
	{
		memcpy(Usart4TxBuf,str,len);
		Usart4SendLen = len;
	}
	else
	{
		return 0;
	}

	USART_ITConfig(USARTx, USART_IT_TC, ENABLE);

	return 1;
}


int bg96_available(pBg96 *bg96)
{
	return (*bg96)->net_buf->available(&((*bg96)->net_buf));
}

unsigned short bg96_read(pBg96 *bg96,unsigned char *buf)
{
	int i = 0;
    unsigned short len = (*bg96)->available(bg96);
    if(len > 0)
    {
        for(i = 0; i < len; i++)
        {
            buf[i] = (*bg96)->net_buf->read(&((*bg96)->net_buf));
        }
    }
    else
    {
//        buf[0] = 0;
        len = 0;
    }
    return len;
}

unsigned char bg96_send(pBg96 *bg96, unsigned char *buffer, unsigned int len)
{
	return (*bg96)->set_AT_QISEND(bg96,buffer, len);
}

unsigned char bg96_restart(pBg96 *bg96)
{
	return 1;
}

//获取当前的连接状态
CONNECT_STATE_E bg96_get_connect_state(pBg96 *bg96)
{
	BG96_STATE_E bg96_state = GET_FAILED;
	CONNECT_STATE_E connect_state = UNKNOW_ERROR;

	bg96_state = (*bg96)->get_AT_QISTATE(bg96);

	if(bg96_state == GET_FAILED)
	{
		connect_state = UNKNOW_ERROR;
	}
	else if(bg96_state == INITIAL || \
			bg96_state == NEED_NEW_CONNECT)
	{
		connect_state = GET_READY;
	}
	else if(bg96_state == CLOSING)
	{
		connect_state = NEED_CLOSE;
	}
	else if(bg96_state == OPENING)
	{
		connect_state = NEED_WAIT;
	}
	else if(bg96_state == CONNECTED)
	{
		connect_state = ON_SERVER;
	}
	else
	{
		connect_state = UNKNOW_ERROR;
	}

	return connect_state;
}

//获取本地IP地址
unsigned char bg96_get_local_ip(pBg96 *bg96,char *msg)
{
	unsigned char ret;
    char buf[128];
	memset(buf,0,128);
    ret = (*bg96)->get_AT_QIACT(bg96,buf);
    if(ret)
    {
        if(strlen(buf) <= 15)
		{
			memcpy(msg,buf,strlen(buf));
		}
    }
    return ret;
}

unsigned char bg96_create_TCP(pBg96 *bg96,char *addr, char *port)
{
	return (*bg96)->set_AT_QIOPEN(bg96,"TCP", addr, port);
}

unsigned char bg96_release_TCP(pBg96 *bg96)
{
	return (*bg96)->set_AT_QICLOSE(bg96);
}

unsigned char bg96_register_UDP(pBg96 *bg96,char *addr, char *port)
{
	return (*bg96)->set_AT_QIOPEN(bg96,"UDP", addr, port);
}

unsigned char bg96_unregister_UDP(pBg96 *bg96)
{
	return (*bg96)->set_AT_QICLOSE(bg96);
}

//基本AT指令测试
unsigned char bg96_set_AT(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//设置回显功能
unsigned char bg96_set_AT_ATE(pBg96 *bg96,char cmd)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("ATE%d\r\n", cmd);
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_UART(pBg96 *bg96,unsigned int baud_rate)
{
	unsigned char ret = 0;

//	RE_SET:
//    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
//    (*bg96)->clear_rx_cmd_buffer(bg96);
//	printf("AT+IPR=%d\r\n",baud_rate);
//    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_5S) == RECEIVED)
//    {
//        if((*bg96)->search_str(gprs,(*bg96)->rx_cmd_buf, "OK") != -1)
//		{
//			(*bg96)->UARTx_Handler.Init.BaudRate = baud_rate;
//			UART_SetConfig(&((*bg96)->UARTx_Handler));
//			ret = 1;
//		}
//    }
//	else
//	{
//		(*gprs)->UARTx_Handler.Init.BaudRate = 115200;
//		UART_SetConfig(&((*gprs)->UARTx_Handler));
//		goto RE_SET;
//	}
//    (*gprs)->bg96_mode = NET_MODE;
//#ifdef BG96_PRINTF_RX_BUF
//	(*gprs)->print_rx_buf(gprs);
//#endif
    return ret;
}

//查询SIM卡状态
unsigned char bg96_get_AT_CPIN(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+CPIN?\r\n");
    if((*bg96)->wait_cmd2(bg96,"READY",TIMEOUT_6S) == RECEIVED)
    {
        if((search_str((unsigned char *)(*bg96)->rx_cmd_buf, "READY") != -1) && \
			(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1))
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//查询信号强度
unsigned char bg96_get_AT_CSQ(pBg96 *bg96)
{
	unsigned char ret = 0;
	unsigned short pos = 0;

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+CSQ\r\n");
    if((*bg96)->wait_cmd2(bg96,"+CSQ: ",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "+CSQ: ") != -1)
		{
			pos = MyStrstr((u8 *)(*bg96)->rx_cmd_buf, "+CSQ: ", 128, 6);
			if(pos != 0xFFFF)
			{
				if((*bg96)->rx_cmd_buf[pos + 7] != ',')
				{
					ret = ((*bg96)->rx_cmd_buf[pos + 6] - 0x30) * 10 +\
						((*bg96)->rx_cmd_buf[pos + 7] - 0x30);
				}
				else
				{
					ret = ((*bg96)->rx_cmd_buf[pos + 6] - 0x30);
				}

				if(ret == 0 && ret == 99)
				{
					ret = 0;
				}
			}
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QCFG1(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QCFG=\"nwscanseq\",03\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QCFG2(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QCFG=\"nwscanmode\",3\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QCFG3(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QCFG=\"iotopmode\",1\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QCFG4(pBg96 *bg96,unsigned char operators)
{
	unsigned char ret = 0;
	unsigned char parm1 = 0;
	unsigned char parm2 = 0;
	unsigned char parm3 = 0;

	switch(operators)
	{
		case 0:			//移动卡
			parm1 = 0;
			parm2 = 10;
			parm3 = 80;
		break;

		case 1:			//联通卡
			parm1 = 0;
			parm2 = 10;
			parm3 = 80;
		break;

		case 2:			//电信卡
			parm1 = 0;
			parm2 = 10;
			parm3 = 10;
		break;

		default:
		break;
	}

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QCFG=\"band\",%d,%d,%d\r\n",parm1,parm2,parm3);
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QCFG5(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QCFG=\"nbsibscramble\",0\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_1S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//设置APN（接入点名称）
unsigned char bg96_set_AT_QICSGP(pBg96 *bg96,unsigned char operators)
{
	unsigned char ret = 0;

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);

	switch(operators)
	{
		case 0:		//移动卡
			printf("AT+QICSGP=1,1,\"CMNBIOT\",\"\",\"\",0\r\n");
		break;

		case 1:		//联通卡
			printf("AT+QICSGP=1,1,\"NBIOT\",\"\",\"\",0\r\n");
		break;

		case 2:		//电信卡
			printf("AT+QICSGP=1,1,\"CTND\",\"\",\"\",0\r\n");
		break;

		default:
		break;
	}
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QIACT(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QIACT=1\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_160S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
            ret = 1;
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}


//获取GPRS模块的IP地址
unsigned char bg96_get_AT_QIACT(pBg96 *bg96,char *list)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QIACT?\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_160S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "+QIACT: 1,1,1,") != -1)
        {
			memcpy(list,&((*bg96)->rx_cmd_buf[17]),(*bg96)->rx_cnt - 9);
            ret = 1;
        }
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//获取当前的连接状态
BG96_STATE_E bg96_get_AT_QISTATE(pBg96 *bg96)
{
	u8 pos1 = 0;

	BG96_STATE_E ret = GET_FAILED;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QISTATE?\r\n");
    if((*bg96)->wait_cmd2(bg96,"OK", TIMEOUT_5S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "+QISTATE") != -1)
		{
			pos1 = MyStrstr((u8 *)(*bg96)->rx_cmd_buf, "\"uart1\"", (*bg96)->rx_cnt, 7);

			if(pos1 != 0xFFFF)
			{
				switch((*bg96)->rx_cmd_buf[pos1 - 8] - 0x30)
				{
					case 0:
						ret = INITIAL;
					break;

					case 1:
						ret = OPENING;
					break;

					case 2:
						ret = CONNECTED;
					break;

					case 3:
						ret = LISTENING;
					break;

					case 4:
						ret = CLOSING;
					break;

					default:

					break;
				}
			}
			else
			{
				ret = GET_FAILED;
			}
		}
		else
		{
			ret = NEED_NEW_CONNECT;
		}
    }
	else
	{
		ret = GET_FAILED;
	}
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//和远端服务器建立单链接
unsigned char bg96_set_AT_QIOPEN(pBg96 *bg96,char *type, char *addr, char *port)
{
	unsigned char ret = 0;
	char buff[128];
	memset(buff,0,128);
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QIOPEN=1,0,\"%s\",\"%s\",%s,0,1\r\n", type, addr, port);
    if((*bg96)->wait_cmd2(bg96,"+QIOPEN:", TIMEOUT_160S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, " 0,0") != -1)
		{
			ret = 1;
		}
    }
	else
	{
		//需要运行QICLOSE
	}
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//关闭和远端服务器的单链接
unsigned char bg96_set_AT_QICLOSE(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QICLOSE=0\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_11S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			ret = 1;
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//在单链接模式下发送数据
unsigned char bg96_set_AT_QISEND(pBg96 *bg96,unsigned char *buffer, unsigned int len)
{
	unsigned char ret = 0;
    u8 state = 0;
//	u8 fail_time = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QISEND=0,%d\r\n", len);
    if((*bg96)->wait_cmd2(bg96,">", TIMEOUT_5S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, ">") != -1)
        {
            state = 1;
        }
    }
	else
	{
		ret = 255;
#ifdef DEBUG_LOG

#endif
	}
    if(state == 1)
    {
        (*bg96)->clear_rx_cmd_buffer(bg96);
		(*bg96)->send_string(bg96,(*bg96)->USARTx,buffer,len);
        if((*bg96)->wait_cmd2(bg96,"SEND OK", TIMEOUT_15S) == RECEIVED)
        {
            if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "SEND OK") != -1)
            {
                state = 2;

				ret = 1;
            }
        }
		else
		{
			ret = 255;
#ifdef DEBUG_LOG

#endif
		}
    }
//	if(state == 2)
//	{
//		while(!(*bg96)->get_AT_QISEND(bg96))
//		{
//			fail_time ++;
//			if(fail_time >= 20)
//			{
//				ret = 254;
//				goto SEND_END;
//			}
//
//			delay_ms(500);
//		}
//
//		ret = 1;
//	}

//	SEND_END:
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//检查缓冲区当中的数据是否已经成功发送到服务器
unsigned char bg96_get_AT_QISEND(pBg96 *bg96)
{
	unsigned char ret = 0;
	u8 pos1 = 0;
	(*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
	printf("AT+QISEND=0,0\r\n");
	if((*bg96)->wait_cmd2(bg96,"+QISEND", TIMEOUT_2S) == RECEIVED)
	{
		if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			pos1 = MyStrstr((u8 *)(*bg96)->rx_cmd_buf, "OK", (*bg96)->rx_cnt, 2);

			if((*bg96)->rx_cmd_buf[pos1 - 5] == 0x30 && (*bg96)->rx_cmd_buf[pos1 - 6] == ',')
			{
				ret = 1;
			}
		}
	}
	else
	{
//		ret = 255;
#ifdef DEBUG_LOG

#endif
	}
//    (*bg96)->bg96_mode = NET_MODE;
//#ifdef BG96_PRINTF_RX_BUF
//	(*bg96)->print_rx_buf(bg96);
//#endif
    return ret;
}

//域名解析
unsigned char bg96_get_AT_QIDNSGIP(pBg96 *bg96,const char *domain, unsigned char **ip)
{
	char msg[32];
    unsigned char ret = 0;
	u8 new_len = 0;
	u8 len = 0;

	memset(msg,0,32);

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
//    printf("AT+QIDNSGIP=1,\"%s\"\r\n", domain);
	printf("AT+QIDNSGIP=1,\"www.baidu.com\"\r\n");
    if((*bg96)->wait_cmd2(bg96,"+QIURC: ",TIMEOUT_5S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "dnsgip") != -1)
        {
			get_str1((unsigned char *)(*bg96)->rx_cmd_buf, "\"", 5, "\"", 6, (unsigned char *)msg);

			new_len = strlen(msg);

			if(new_len != 0)
			{
				if(*ip == NULL)
				{
					*ip = (u8 *)mymalloc(sizeof(u8) * len + 1);
				}

				if(*ip != NULL)
				{
					len = strlen((char *)*ip);

					if(len == new_len)
					{
						memset(*ip,0,new_len + 1);
						memcpy(*ip,msg,new_len);
						ret = 1;
					}
					else
					{
						myfree(*ip);
						*ip = (u8 *)mymalloc(sizeof(u8) * new_len + 1);
						if(ip != NULL)
						{
							memset(*ip,0,new_len + 1);
							memcpy(*ip,msg,new_len);
							len = new_len;
							new_len = 0;
							ret = 1;
						}
					}
				}
			}
        }
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//PING
unsigned char bg96_get_AT_QPING(pBg96 *bg96,const char *host, char *msg)
{
	unsigned char ret = 0;

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QPING=1,\"%s\",4,4\r\n", host);
    if((*bg96)->wait_cmd2(bg96,"OK",TIMEOUT_5S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
        {
            get_str3((unsigned char *)(*bg96)->rx_cmd_buf, (unsigned char *)msg, (*bg96)->rx_cnt);
            ret = 1;
        }
        else
            ret = 0;
    }
    (*bg96)->bg96_mode = NET_MODE;

#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_get_AT_GSN(pBg96 *bg96)
{
	unsigned char ret = 0;
	char buf[32];

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+GSN\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			memset(buf,0,32);

			get_str1((unsigned char *)(*bg96)->rx_cmd_buf, "\r\n", 1, "\r\n", 2, (unsigned char *)buf);

			if(strlen(buf) == 15)
			{
				if((*bg96)->imei == NULL)
				{
					(*bg96)->imei = (char *)mymalloc(sizeof(char) * 16);
				}
				if((*bg96)->imei != NULL)
				{
					memset((*bg96)->imei,0,16);
					memcpy((*bg96)->imei,buf,15);

					ret = 1;
				}
			}
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QGPS(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QGPS=1,30,50,0,1\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_2S) == RECEIVED)
    {
		if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			ret = 1;
		}
		else if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "ERROR: 504") != -1)
		{
			ret = 1;
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QGPSLOC(pBg96 *bg96,char *msg)
{
	unsigned char ret = 0;

    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QGPSLOC=0\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "QGPSLOC: ") != -1)
			{
				get_str1((unsigned char *)(*bg96)->rx_cmd_buf, "+QGPSLOC: ", 1, "\r\n\r\nOK", 1, (unsigned char *)msg);

				ret = 1;
			}
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QGPSEND(pBg96 *bg96)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
    printf("AT+QGPSEND\r\n");
    if((*bg96)->wait_cmd1(bg96,TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK") != -1)
		{
			ret = 1;
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

unsigned char bg96_set_AT_QNTP(pBg96 *bg96,char *server,unsigned short port,char *msg)
{
	unsigned char ret = 0;
    (*bg96)->wait_bg96_mode(bg96,CMD_MODE);
    (*bg96)->clear_rx_cmd_buffer(bg96);
//    printf("AT+QNTP=1,\"%s\",%d,0\r\n",server,port);
	printf("AT+QNTP=1,\"cn.ntp.org.cn\",123,0\r\n");
    if((*bg96)->wait_cmd2(bg96,"+QNTP:",TIMEOUT_2S) == RECEIVED)
    {
        if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, ",\"20") != -1)
		{
			get_str1((unsigned char *)(*bg96)->rx_cmd_buf, "\"", 1, "\"", 2, (unsigned char *)msg);

			ret = 1;
		}
    }
    (*bg96)->bg96_mode = NET_MODE;
#ifdef BG96_PRINTF_RX_BUF
	(*bg96)->print_rx_buf(bg96);
#endif
    return ret;
}

//清空AT指令接收缓存
void bg96_clear_rx_cmd_buffer(pBg96 *bg96)
{
	uint16_t i;
    for(i = 0; i < CMD_DATA_BUFFER_SIZE; i++)
    {
        (*bg96)->rx_cmd_buf[i] = 0;
    }
    (*bg96)->rx_cnt = 0;
}

void bg96_print_rx_buf(pBg96 *bg96)
{
	UsartSendString(USART1,(unsigned char *)(*bg96)->rx_cmd_buf,(*bg96)->rx_cnt);
}

void bg96_print_cmd(pBg96 *bg96,CMD_STATE_E cmd)
{

}

CMD_STATE_E bg96_wait_cmd1(pBg96 *bg96,unsigned int wait_time)
{
	unsigned int time = GetSysTick1ms();
	unsigned int time_now = 0;
    (*bg96)->cmd_state = WAITING;

    while(1)
    {
		time_now = GetSysTick1ms();
        if((time_now - time) > wait_time)
        {
            (*bg96)->cmd_state = TIMEOUT;
            break;
        }

        if(
            search_str((unsigned char *)(*bg96)->rx_cmd_buf, "OK"   ) != -1  || \
            search_str((unsigned char *)(*bg96)->rx_cmd_buf, "FAIL" ) != -1  || \
            search_str((unsigned char *)(*bg96)->rx_cmd_buf, "ERROR") != -1
        )
        {
            while(GetSysTick1ms() - (*bg96)->last_time < 20);
				(*bg96)->cmd_state = RECEIVED;
			break;
        }
		delay_ms(10);
    }
    (*bg96)->print_cmd(bg96,(*bg96)->cmd_state);

    return (*bg96)->cmd_state;
}

CMD_STATE_E bg96_wait_cmd2(pBg96 *bg96,const char *spacial_target, unsigned int wait_time)
{
	unsigned int time = GetSysTick1ms();
	unsigned int time_now = 0;
	(*bg96)->cmd_state = WAITING;

    while(1)
    {
		time_now = GetSysTick1ms();
        if((time_now - time) > wait_time)
        {
            (*bg96)->cmd_state = TIMEOUT;
            break;
        }

        else if(search_str((unsigned char *)(*bg96)->rx_cmd_buf, (unsigned char *)spacial_target) != -1)
        {
            while(GetSysTick1ms() - (*bg96)->last_time < 20);
				(*bg96)->cmd_state = RECEIVED;
            break;
        }
		else if((*bg96)->break_out_wait_cmd == 1)
		{
			(*bg96)->break_out_wait_cmd = 0;
			break;
		}
		delay_ms(10);
    }
    (*bg96)->print_cmd(bg96,(*bg96)->cmd_state);

    return (*bg96)->cmd_state;
}

unsigned char bg96_wait_bg96_mode(pBg96 *bg96,GPRS_MODE_E mode)
{
	unsigned char ret = 0;

	ret = 1;
	if(GetSysTick1ms() - (*bg96)->last_time > 20)
	{
		(*bg96)->bg96_mode = mode;
	}
    else
    {
        while(GetSysTick1ms() - (*bg96)->last_time < 20);
        (*bg96)->bg96_mode = mode;
    }
    return ret;
}

//从串口获取一个字符
void bg96_get_char(pBg96 *bg96)
{
	unsigned char c;

	c = USART_ReceiveData((*bg96)->USARTx);
	(*bg96)->last_time = GetSysTick1ms();

	if((*bg96)->bg96_mode == CMD_MODE)
	{
		(*bg96)->rx_cmd_buf[(*bg96)->rx_cnt] = c;
		if((*bg96)->rx_cnt ++ > CMD_DATA_BUFFER_SIZE)
		{
			(*bg96)->rx_cnt = 0;
		}
		(*bg96)->cmd_state = RECEIVING;
	}
	(*bg96)->net_data_state_process(bg96,c);
}

void bg96_uart_interrupt_event(pBg96 *bg96)
{
	(*bg96)->get_char(bg96);
}

void bg96_net_data_state_process(pBg96 *bg96,char c)
{
	static NET_DATA_STATE_E net_data_state = NEED_I;
	static u8 yin_cnt = 0;

	switch((unsigned char)net_data_state)
	{
		case (unsigned char)NEED_PLUS:
			if(c == '+')
			{
				net_data_state  = NEED_Q;
				yin_cnt = 0;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_Q:
			if(c == 'Q')
			{
				net_data_state = NEED_I;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_I:
			if(c == 'I')
			{
				net_data_state = NEED_U;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_U:
			if(c == 'U')
			{
				net_data_state = NEED_R;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_R:
			if(c == 'R')
			{
				net_data_state = NEED_C;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_C:
			if(c == 'C')
			{
				net_data_state = NEED_MAO;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_MAO:
			if(c == ':')
			{
				net_data_state = NEED_KONG;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_KONG:
			if(c == ' ')
			{
				net_data_state = NEED_YIN;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_YIN:
			if(c == '\"')
			{
				if(yin_cnt == 0)
				{
					net_data_state = NEED_r;
					yin_cnt = 1;
				}
				else
				{
					net_data_state = NEED_DOU;
				}
			}
			else
			{
				net_data_state = NEED_PLUS;
				yin_cnt = 0;
			}

			break;

		case (unsigned char)NEED_r:
			if(c == 'r')
			{
				net_data_state = NEED_e;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_e:
			if(c == 'e')
			{
				net_data_state = NEED_c;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_c:
			if(c == 'c')
			{
				net_data_state = NEED_v;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_v:
			if(c == 'v')
			{
				net_data_state = NEED_YIN;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_DOU:
			if(c == ',')
			{
				net_data_state = NEED_LEN_DATA;
			}
			else
			{
				net_data_state = NEED_PLUS;
			}
			break;

		case (unsigned char)NEED_LEN_DATA:
			if(c >= '0' && c <= '9')
			{
				net_temp_data_rx_buf[(*bg96)->net_data_rx_cnt++] = c;
			}
			else if(c == 0x0D)
			{

			}
			else if(c == 0x0A)/*<len>:<data>*/
			{
				net_temp_data_rx_buf[(*bg96)->net_data_rx_cnt++] = '\0';
				(*bg96)->net_data_len = atoi((const char *)net_temp_data_rx_buf);
				net_data_state = NEED_USER_DATA;
				(*bg96)->net_data_rx_cnt = 0;
			}
			else if(c == ',') /* +IPD,<id>,<len>:<data> */
			{
				(*bg96)->net_data_id = net_temp_data_rx_buf[0] - 48; //char to int
				(*bg96)->net_data_rx_cnt = 0;
			}
			else
			{
				(*bg96)->net_data_rx_cnt = 0;
				net_data_state = NEED_PLUS;
			}
			break;
//		case (unsigned char)NEED_COLON:
//			if(c == ':')
//				net_data_state = NEED_USER_DATA;
//			else
//				net_data_state = NEED_PLUS;
//			break;
		case (unsigned char)NEED_USER_DATA:
			if((*bg96)->net_data_rx_cnt < (*bg96)->net_data_len)
			{
				(*bg96)->net_buf->write(&((*bg96)->net_buf),c);
				(*bg96)->net_data_rx_cnt++;
			}
			else
			{
				(*bg96)->net_data_rx_cnt = 0;
				net_data_state = NEED_PLUS;
			}
			break;
		default:
			net_data_state = NEED_PLUS;
			(*bg96)->net_data_rx_cnt = 0;
			break;
	}
}































