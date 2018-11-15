#ifndef __TCP_H
#define	__TCP_H

#include "sys.h"
#include "bg96.h"


#define Tcp struct TCP
typedef struct TCP *pTcp;
struct TCP
{
	unsigned char 	(*connect)(pTcp *tcp,char *remote_ip, char *remote_port);
	unsigned char 	(*close)(pTcp *tcp);
	unsigned short 	(*send)(pTcp *tcp, unsigned char *buf, unsigned short len);
	unsigned short 	(*read)(pTcp *tcp,unsigned char *buf);
	
	pBg96 bg96;
};


extern pTcp tcp;

unsigned char 	Tcp_Init(pBg96 *bg96,pTcp *tcp);
void 			Tcp_UnInit(pTcp *tcp);
unsigned char 	tcp_connect(pTcp *tcp,char *remote_ip, char *remote_port);
unsigned char 	tcp_close(pTcp *tcp);
unsigned short 	tcp_send(pTcp *tcp, unsigned char *buf, unsigned short len);
unsigned short 	tcp_read(pTcp *tcp,unsigned char *buf);













































#endif
