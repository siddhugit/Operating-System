/*
 * pipeDemo.c
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 *      Implements pipe demo functions ( echo, sort, unique and print )
 */
#include <stdio.h>
#include <stdlib.h>
#include "ustream.h"
#include "fstring.h"

#define CHAR_EOF 4

//echo command
int pipe_demo1(int argc, char *argv[])
{
	char tempstr[1024];
	int i;
	if(argc > 1)
	{	
		sprintf(tempstr,"%s",argv[1]);uprintf(tempstr);
		for(i = 2;i < argc;++i)
		{
			sprintf(tempstr," %s",argv[i]);uprintf(tempstr);
		}
	}
	sprintf(tempstr,"\r\n");uprintf(tempstr);
	return 0;
}
static const int LINE_MAX = 1024;
static const int WORD_MAX = 256;

//print command
int pipe_demo2(int argc, char *argv[])
{
	char line[LINE_MAX + 1]; //command line temporary buffer
	char c;
	int pos = 0;
	while( ( c = ugetc()) != CHAR_EOF && pos < LINE_MAX) 
	{
		if ( c == '\r')
		{
			break;
		}
		line[pos++] = c;
	}
	line[pos] = '\0';
	uprintf(line);
	return 0;
}

static int less(const void *t1, const void *t2)
{
   return (*(int*)t1 - *(int*)t2);
}

#define NUM_INTEGERS 128

//sort numbers
int pipe_demo3(int argc, char *argv[])
{
	char line[LINE_MAX + 1]; //command line temporary buffer
	char c;
	int pos = 0;
	while( ( c = ugetc()) != CHAR_EOF && pos < LINE_MAX) 
	{
		if ( c == '\r')
		{
			break;
		}
		line[pos++] = c;
	}
	line[pos] = '\0';
	
	char *cstr,*token;
	const char* delim = " ";
	char* str = line;
	int found;
	int count = 0;
	int nums[NUM_INTEGERS];
	for (cstr = str; ; cstr = NULL)
	{
		token = strtokenizer(cstr,delim,&found);
		if (token == NULL)
			break;
		nums[count++] = strtoint(token);
	}		
	qsort(nums, count, sizeof(int), less);//library function uses quick sort
	char tempstr[1024];
	int i;
	for(i = 0 ; i < count; ++i ) 
    {   
	   sprintf(tempstr,"%d ", nums[i]);
	   uprintf(tempstr);
    }
	sprintf(tempstr,"\r\n");uprintf(tempstr);
	return 0;
}

//unique command
int pipe_demo4(int argc, char *argv[])
{
	char word[WORD_MAX]; //command line temporary buffer
	char c;
	int pos = 0,count = 0,prev;
	char tempstr[1024];
	while( ( c = ugetc()) != CHAR_EOF && pos < LINE_MAX) 
	{
		if ( c == '\r')
		{
			break;
		}
		else if(c == ' ')
		{
			word[pos] = '\0';
			pos = 0;
			int curr = strtoint(word);
			if(count == 0)
			{
				sprintf(tempstr,"%d ", curr);
				uprintf(tempstr);
			}
			else
			{
				if(prev != curr)
				{
					sprintf(tempstr,"%d ", curr);
					uprintf(tempstr);
				}
			}
			prev = curr;
			++count;
		}
		else
		{
			word[pos++] = c;
		}
	}
	sprintf(tempstr,"\r\n");uprintf(tempstr);
	return 0;
}
