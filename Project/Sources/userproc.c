/*
 * userproc.c
 *
 *  Created on: Dec 3, 2015
 *      Author: URM
 */
#include <stdio.h>
#include "ustream.h"
#include "svc.h"

#define CHAR_EOF 4

//static const unsigned long int delayCount = 0x7ffff;
static const unsigned long int delayCount = 0x1FFFF;

static void delay(unsigned long int limit) 
{
	unsigned long int i;
	for(i = 0; i < limit; i++) 
	{
	}
}

void demoProc1(int argc,char *argv[])
{
	int wfd = svc_open("/lcd",'w');
	if(wfd > 0)
	{
		char buff;
		while((buff = ugetc()) != CHAR_EOF)
		{
			svc_write(wfd,buff);
		}
		svc_close(wfd);
	}
}

void demoProc2(int argc,char *argv[])
{
	char tempstr[1024];
	int fd1 = svc_open("/btn/sw1",'r');
	int fd2 = svc_open("/btn/sw2",'r');
	while(1)
	{
		char buff1,buff2;
		int bytesread1 = svc_read(fd1,&buff1);
		int bytesread2 = svc_read(fd2,&buff2);
		if(bytesread2 == 1 && buff2 == '1')
		{
			sprintf(tempstr,"Switch2 Pressed\r\n");uprintf(tempstr);
			delay(delayCount);
		}
		else if(bytesread1 == 1 && buff1 == '1')
		{
			break;
		}
	}
}

static volatile char LEDSTATE = 0;

void setLedState(char state)
{
	LEDSTATE = state;
}
static char getLedState()
{
	return LEDSTATE;
}
void demoProc3(int argc,char *argv[])
{
	char ledState = getLedState();
	svc_pdbDemo(1);
	int fd = svc_open("/tsr/orange",'r');
	while(1)
	{
		char buff;
		svc_read(fd,&buff);
		if(buff == '1')
		{
			break;
		}
		if(ledState != getLedState())
		{
			ledState = getLedState();
			svc_pdbDemo(1);
		}
	}
	svc_close(fd);
}

