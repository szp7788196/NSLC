#include "task_net.h"
#include "common.h"
#include "delay.h"
#include "tcp.h"
#include "net_protocol.h"
#include "rtc.h"


TaskHandle_t xHandleTaskNET = NULL;

CONNECT_STATE_E ConnectState = UNKNOW_ERROR;	//bg96的连接状态
u8 SignalIntensity = 99;						//bg96的信号强度

SensorMsg_S *p_tSensorMsgNet = NULL;			//用于装在传感器数据的结构体变量

u8 perccennnt = 0;
//u8 *IpAddress = NULL;
unsigned portBASE_TYPE NET_Satck;
void vTaskNET(void *pvParameters)
{
	s8 res = 0;
	time_t times_sec = 0;

	BG96_InitStep1(&bg96);
	BG96_InitStep2(&bg96);

	Tcp_Init(&bg96,&tcp);

	p_tSensorMsgNet = (SensorMsg_S *)mymalloc(sizeof(SensorMsg_S));

	while(1)
	{
		if(ConnectState == ON_SERVER)
		{
			if(GetSysTick1s() - times_sec >= 5)
			{
				times_sec = GetSysTick1s();
				ConnectState = bg96->get_connect_state(&bg96);
				SignalIntensity = bg96->get_AT_CSQ(&bg96);
				GetGpsInfo(&GpsInfo,&GetGPSOK,&GetTimeOK);
//				GetTimeInfo("cn.ntp.org.cn",123, &GetTimeOK);

//				if(IpAddress == NULL && ServerDomain != NULL)
//				{
//					bg96->get_AT_QIDNSGIP(&bg96,(char *)ServerDomain, &IpAddress);
//				}
			}
		}
		else
		{
			ConnectState = bg96->get_connect_state(&bg96);
			SignalIntensity = bg96->get_AT_CSQ(&bg96);
		}

		switch((u8)ConnectState)
		{
			case (u8)UNKNOW_ERROR:					//未知错误，重启模块
				RE_RESET:
				ConnectState = UNKNOW_ERROR;
				BG96_InitStep2(&bg96);
			break;

			case (u8)GET_READY:						//模块已经就绪
				res = TryToConnectToServer();

				if(res == 0)						//打开链接失败
				{
					goto RE_RESET;					//复位模块
				}
			break;

			case (u8)NEED_CLOSE:					//需要关闭移动场景
				tcp->close(&tcp);
			break;

			case (u8)NEED_WAIT:						//延时等待
				delay_ms(2000);
			break;

			case (u8)ON_SERVER:						//已经连接到服务器
				res = OnServerHandle();

				if(res == -1)
				{
					goto RE_RESET;					//复位模块
				}
			break;

			default:

			break;
		}

		delay_ms(100);

		perccennnt = mem_perused();
		NET_Satck = uxTaskGetStackHighWaterMark(NULL);
	}
}

//尝试连接服务器
u8 TryToConnectToServer(void)
{
	u8 ret = 0;

	if(ServerIP != NULL && ServerPort != NULL)
	{
		ret = tcp->connect(&tcp,(char *)ServerIP,(char *)ServerPort);
	}

	return ret;
}

//在线处理进程
s8 OnServerHandle(void)
{
	s8 ret = 0;
	s16 len = 0;
	u8 out_buf[512];

	SendSensorData_HeartBeatPacket();								//向服务器定时发送传感器数据和心跳包

	len = NetDataFrameHandle(&tcp,out_buf,HoldReg,ConnectState);	//读取并解析服务器下发的数据包

	if(len > 0)
	{
		len = tcp->send(&tcp, out_buf, len);						//把数据发送到服务器
	}
	else if(len < 0)
	{
		ret = -1;													//一分钟内没有收到数据，强行关闭连接
	}

	return ret;
}

//向服务器定时发送传感器数据和心跳包
void SendSensorData_HeartBeatPacket(void)
{
	static time_t times_sec = 0;
	BaseType_t xResult;
	u8 send_len = 0;
	u8 sensor_data_len = 0;

	u8 sensor_buf[128];
	u8 send_buf[512];

	xResult = xQueueReceive(xQueue_sensor,
							(void *)p_tSensorMsgNet,
							(TickType_t)pdMS_TO_TICKS(50));
	if(xResult == pdPASS)
	{
		memset(send_buf,0,512);
		memset(sensor_buf,0,128);

		if(DeviceUUID != NULL)			//发送普通数据
		{
			sensor_data_len = UnPackSensorData(p_tSensorMsgNet,sensor_buf);

			send_len = PackNetData(0xE0,sensor_buf,sensor_data_len,send_buf);
		}
		else							//发送UUID请求
		{
			send_len = PackNetData(0xF0,NULL,0,send_buf);
		}
	}
	else if(GetSysTick1s() - times_sec >= 20 && UpLoadINCL > 20)	//只有在数据上传周期大于20秒的时候才发送心跳包，间隔是20秒
	{
		times_sec = GetSysTick1s();

		memset(send_buf,0,512);
		memset(sensor_buf,0,128);

		send_len = PackNetData(0xE1,sensor_buf,0,send_buf);
	}
//	else if(GetTimeOK == 3)				//发送对时请求
//	{
//		send_len = PackNetData(0xF1,sensor_buf,0,send_buf);
//	}

	if(send_len >= 1)
	{
		send_len = tcp->send(&tcp, send_buf, send_len);
	}
}

//获取GPS信息
u8 GetGpsInfo(u8 **gps_info,u8 *gps_flag,u8 *time_flag)
{
	u8 ret = 0;
	u8 buf[80];
	u8 jing[16];
	u8 wei[16];
	u16 len = 0;

	struct tm tm_time;
	time_t time_s = 0;
	u8 buf_len = 0;

	if(*gps_flag != 1 || *time_flag != 1)
	{
		memset(buf,0,80);

		if(bg96->set_AT_QGPS(&bg96))						//开启GPS
		{
			if(bg96->set_AT_QGPSLOC(&bg96,(char *)buf))		//获取信息
			{
//				bg96_set_AT_QGPSEND(&bg96);					//关闭GPS

				memset(jing,0,16);
				memset(wei,0,16);
				get_str1(buf, ",", 1, ",", 2, jing);
				get_str1(buf, ",", 2, ",", 3, wei);

				len = strlen((char *)jing) + strlen((char *)wei);

				if(*gps_info == NULL)
				{
					*gps_info = (u8 *)mymalloc(len + 1);
				}
				else if(*gps_info != NULL)
				{
					myfree(*gps_info);
					*gps_info = (u8 *)mymalloc(sizeof(u8) * len + 1);
				}

				if(*gps_info != NULL)
				{
					memset(*gps_info,0,len + 1);

					strcat((char *)*gps_info,(char *)jing);
					strcat((char *)*gps_info,(char *)wei);

//					*gps_flag = 1;

					ret = 1;
				}

				if(*time_flag != 1)
				{
					buf_len = strlen((char *)buf);

					tm_time.tm_year = 2000 + (buf[buf_len - 5] - 0x30) * 10 + buf[buf_len - 4] - 0x30 - 1900;
					tm_time.tm_mon  = (buf[buf_len - 7] - 0x30) * 10 + buf[buf_len - 6] - 0x30 - 1;
					tm_time.tm_mday = (buf[buf_len - 9] - 0x30) * 10 + buf[buf_len - 8] - 0x30;

					tm_time.tm_hour = (buf[0] - 0x30) * 10 + buf[1] - 0x30;
					tm_time.tm_min  = (buf[2] - 0x30) * 10 + buf[3] - 0x30;
					tm_time.tm_sec  = (buf[4] - 0x30) * 10 + buf[5] - 0x30;

					time_s = mktime(&tm_time);

					time_s = time_s + (8 * 3600);

					SyncTimeFromNet(time_s);

					*time_flag = 1;
				}
			}
		}
	}

	return ret;
}

//从指定的NTP服务器获取时间
u8 GetTimeInfo(char *server,u8 port, u8 *flag)
{
	u8 ret = 0;
	struct tm tm_time;
	time_t time_s = 0;
	char buf[32];

	if(*flag != 1)
	{
		memset(buf,0,32);

		if(bg96->set_AT_QNTP(&bg96,server,port,buf))
		{
			tm_time.tm_year = (buf[0] - 0x30) * 1000 + (buf[1] - 0x30) * 100 + (buf[2] - 0x30) * 10 + buf[3] - 0x30 - 1990;
			tm_time.tm_mon = (buf[5] - 0x30) * 10 + buf[6] - 0x30 - 1;
			tm_time.tm_mday = (buf[8] - 0x30) * 10 + buf[9] - 0x30;

			tm_time.tm_hour = (buf[11] - 0x30) * 10 + buf[12] - 0x30;
			tm_time.tm_min = (buf[14] - 0x30) * 10 + buf[15] - 0x30;
			tm_time.tm_sec = (buf[17] - 0x30) * 10 + buf[18] - 0x30;

			time_s = mktime(&tm_time);

			time_s += 57600;

			SyncTimeFromNet(time_s);

			*flag = 1;
			ret = 1;
		}
	}

//	if(*flag != 1)
//	{
//		*flag = 3;
//	}

	return ret;
}






































