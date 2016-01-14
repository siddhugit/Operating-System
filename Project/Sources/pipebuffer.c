/*
 * pipebuffer.c
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 *      Implements circular data structure from pipe buffer
 */

#include <stdlib.h>
#include <memory.h>
#include "pipebuffer.h"
#include "error.h"
#include "proc.h"

#define PIPEBUFFER_SIZE 1024

#define CHAR_EOF 4

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

struct PipeBuffer *pipeBufferCreate()
{
	struct PipeBuffer *pb = myMalloc(sizeof(struct PipeBuffer));
    pb->data = myMalloc(PIPEBUFFER_SIZE);
    pb->start = 0;
    pb->end = 0;
	pb->size = 0;
	pb->refCount = 2;//A pipe is owned by two processes
	return pb;
}

void pipeBufferDestroy(struct PipeBuffer *pb)
{
	if(pb) 
	{
		--pb->refCount;//decrement ref count
		if(pb->refCount == 0)//free resources when ref count is zero
		{
			systemFree(pb->data);
			systemFree(pb);
		}
    }
}

int pipeBufferGetChar(struct PipeBuffer *pb,char *buff)
{
	di();
    while(pb->size == 0)
	{
    	if(pb->refCount == 1)
		{
			//write end is closed return EOF
    		*buff = CHAR_EOF;
    		ei();
			return 1;
		}
    	ei();
    	yield();//loops until buffer is non-empty, yield CPU
    	di();
	}
    char result = pb->data[pb->start];
	++pb->start;
    if(pb->start == PIPEBUFFER_SIZE)
	{
		pb->start = 0;   
	}
    --pb->size;
    *buff = result;
    ei();
	return 1;
}

int pipeBufferPutChar(struct PipeBuffer *pb,char ch)
{
    di();
	while(pb->size == PIPEBUFFER_SIZE)
	{
    	if(pb->refCount == 1)
		{
			//read end is closed
    		my_errno = EMYPIPE; 
    		ei();
    		return 0;
		}
    	ei();
        yield();//loops until buffer is full, yield CPU
        di();
	}
	pb->data[pb->end] = ch;
    ++pb->end;
    if(pb->end == PIPEBUFFER_SIZE)
	{
        pb->end = 0;
	}
    ++pb->size;
    ei();
    return 1;
}
