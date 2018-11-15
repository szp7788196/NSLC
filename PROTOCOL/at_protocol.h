#ifndef __AT_PROTOCOL_H
#define __AT_PROTOCOL_H

#include "sys.h"
#include "common.h"

#define AT_MAX_NUM	26

#define RST				0
#define GMR				1
#define DEVNAME			2
#define DEVID			3
#define UUID			4
#define OPERATORS		5
#define APN				6
#define DOMAIN			7
#define IPADDRESS		8
#define PORT			9
#define INCL			10
#define INTFC			11
#define TIMEOFFSET		12
#define PERCENT			13
#define CSQ				14
#define INCURRENT		15
#define INVOLTAGE		16
#define OUTCURRENT		17
#define OUTVOLTAGE		18
#define TEMP			19
#define HUMI			20
#define LUMEN			21
#define TIME			22
#define GPS				23
#define MODE			24
#define PWRCTL			25


typedef struct AT_Command
{
	char *cmd;
	u8 len;
	u8 id;
}AT_Command_S;



extern AT_Command_S AT_CommandBuf[AT_MAX_NUM];

/******************************************************************************************
/                                    通讯错误码
/	
/	
/	
/	
/	
/	
/	
/
******************************************************************************************/
void AT_CommandInit(void);
u16 AT_CommandDataAnalysis(u8 *buf,u16 len,u8 *outbuf,u8 *hold_reg);
u8 PackAT_CommandRespone(u8 *inbuf,u8 err_code,u8 *respbuf,u8 *outbuf);


u8 AT_CommandRST(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandGMR(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandDEVNAME(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandDEVID(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandUUID(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandOPERATORS(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandAPN(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandDOMAIN(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandIPADDRESS(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandPORT(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandINCL(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandINTFC(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandTIMEOFFSET(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandPERCENT(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandCSQ(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandINCURRENT(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandINVOLTAGE(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandOUTCURRENT(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandOUTVOLTAGE(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandTEMP(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandHUMI(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandLUMEN(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandTIME(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandGPS(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandMODE(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);
u8 AT_CommandPWRCTL(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf);









































#endif
