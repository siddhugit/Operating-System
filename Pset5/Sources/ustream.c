/*
 * ustream.c
 *
 *  Created on: Nov 1, 2015
 *      Author: URM
 */
#include "fstring.h"
#include "svc.h"


static int UART_FD;

void uprintf(const char* str)
{
	int i;
	int len = str_len(str);
	for(i = 0;i < len; ++i)
	{
		svc_write(UART_FD,str[i]);
	}
}
void uinit()
{
	UART_FD = svc_open("/uart",'w');
}
char ugetc()
{
	char buff;
	svc_read(UART_FD,&buff);
	svc_write(UART_FD,buff);
	if(buff == '\r')
	{
		svc_write(UART_FD,'\n');
	}
	return buff;
}
