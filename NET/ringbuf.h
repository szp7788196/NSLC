#ifndef __RINGBUF_H
#define __RINGBUF_H

#define BUF_NULL	0x00000000

#define RingBuf struct RINGBUF
typedef struct RINGBUF *pRingBuf;

struct RINGBUF
{
	unsigned char (*write)(pRingBuf *buf, unsigned char c);
	unsigned char (*read)(pRingBuf *buf);
	int (*available)(pRingBuf *buf);
	void (*clear)(pRingBuf *buf);
	
	volatile unsigned int head;
	volatile unsigned int tail;
	unsigned int max;
	unsigned char *buf;
	
};

unsigned char RingBuf_Init(pRingBuf *buf, unsigned int lenght);
unsigned char ringbuf_write(pRingBuf *buf, unsigned char c);
unsigned char ringbuf_read(pRingBuf *buf);
int ringbuf_available(pRingBuf *buf);
void ringbuf_clear(pRingBuf *buf);














#endif


