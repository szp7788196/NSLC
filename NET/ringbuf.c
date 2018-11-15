#include "ringbuf.h"
#include "FreeRTOS.h"

unsigned char RingBuf_Init(pRingBuf *buf, unsigned int lenght)
{
	unsigned char ret = 0;
	
	(*buf) = (pRingBuf)mymalloc(sizeof(RingBuf));
	if((*buf) == BUF_NULL)
	{
		return 0;
	}
	
	(*buf)->head = 0;
	(*buf)->tail = 0;
	(*buf)->max = lenght;
	(*buf)->write = ringbuf_write;
	(*buf)->read = ringbuf_read;
	(*buf)->available = ringbuf_available;
	(*buf)->clear = ringbuf_clear;
	
	if(lenght == 0)
	{
		return 0;
	}
	else if(lenght > 0)
	{
		(*buf)->buf = (unsigned char *)mymalloc(sizeof(unsigned char) * lenght);
		if((*buf)->buf == BUF_NULL)
		{
			return 0;
		}
		ret = 1;
	}
	return ret;
}

unsigned char ringbuf_write(pRingBuf *buf, unsigned char c)
{
	unsigned char res = 0;
	
	unsigned int i = 0;
	i = ((*buf)->head + 1) % (*buf)->max;
	
	if(i == (*buf)->tail)
	{
		if(((*buf)->head + 1) < (*buf)->max)
		{
			(*buf)->head += 1;
		}
		else if(((*buf)->head + 1) == (*buf)->max)
		{
			(*buf)->head = 0;
		}
	}
	
	(*buf)->buf[(*buf)->head] = c;
	(*buf)->head = i;
	
	return res;
}

unsigned char ringbuf_read(pRingBuf *buf)
{
	if((*buf)->head == (*buf)->tail)
	{
		return (char)-1;
	}
	else
	{
		unsigned char c = (*buf)->buf[(*buf)->tail];
		(*buf)->tail = (unsigned int)((*buf)->tail + 1) % (*buf)->max;
		return c;
	}
}

int ringbuf_available(pRingBuf *buf)
{
	return (int)((*buf)->max + (*buf)->head - (*buf)->tail) % (*buf)->max;
}

void ringbuf_clear(pRingBuf *buf)
{
	int i = 0;
	
	(*buf)->head = 0;
	(*buf)->tail = 0;
	
	for(i = 0; i < (*buf)->max; i ++)
	{
		(*buf)->buf[i] = 0;
	}
}















































