/*
 * ustream.c
 *
 *  Created on: Nov 1, 2015
 *      Author: URM
 */
#include "fstring.h"
#include "svc.h"
#include "proc.h"


static int UART_FD;


static void consoleWrite(const char* str,int fd)
{
	int i;
	int len = str_len(str);
	for(i = 0;i < len; ++i)
	{
		svc_write(fd,str[i]);
	}
}

static char consoleRead()
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

void uinit()
{
	UART_FD = svc_open("/uart",'w');
}

void uprintf(const char* str)
{
	int fd = getStdFileOut();
	consoleWrite(str,fd);
}

char ugetc()
{
	int fd = getStdFileIn();
	if(fd == UART_FD)
	{
		return consoleRead();
	}
	else
	{
		char buff;
		svc_read(fd,&buff);
		return buff;
	}
}

int getConsoleFD()
{
	return UART_FD;
}
