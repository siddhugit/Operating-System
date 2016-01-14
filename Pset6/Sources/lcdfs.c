/*
 * lcdfs.c
 *
 *  Created on: Oct 31, 2015
 *      Author: URM
 */

#include <stdlib.h>
#include "lcdfs.h"
#include "lcdc.h"
#include "lcdcConsole.h"
#include "svc.h"
#include "memory.h"

#define CHAR_EOF 4

struct console console;

//Button stream class
struct lcd_stream
{
	void* minor;
};

void* lcd_open(const char *name,char mode)
{
	struct lcd_stream *stream = myMalloc(sizeof(struct lcd_stream));
	stream->minor = NULL;
	return stream;
}
void lcd_close(void *fp)
{
	myFree(fp);
}
void lcd_create(const char *name)
{
}
void lcd_delete(const char *name)
{
}
int lcd_read(void *fp,char* buff)
{
	return 0;
}
int lcd_write(void *fp,char c)
{
	if(c == CHAR_EOF) 
	{
		return 0;
	}
	if(c == '\r')
	{
		lcdcConsolePutc(&console, '\n');
	}
	else
	{
		lcdcConsolePutc(&console, c);
	}
	return 1;
}
char** lcd_list()
{
	return NULL;
}


