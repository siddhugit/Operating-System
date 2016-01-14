/*
 * pipefs.c
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 *      Implements virtual file system interface for pipe
 */


#include <stdlib.h>
#include "svc.h"
#include "memory.h"
#include "pipebuffer.h"

//Button stream class
struct pipe_stream
{
	void* minor;
};

void* pipe_open(const char *name,char mode)
{
	struct PipeBuffer *stream = pipeBufferCreate();
	return stream;
}
void pipe_close(void *fp)
{
	pipeBufferDestroy(fp);
}
void pipe_create(const char *name)
{
}
void pipe_delete(const char *name)
{
}
int pipe_read(void *fp,char* buff)
{
	int ret = pipeBufferGetChar((struct PipeBuffer *)fp,buff);
	return ret;
}
int pipe_write(void *fp,char c)
{
	int ret = pipeBufferPutChar((struct PipeBuffer *)fp,c);
	return ret;
}
char** pipe_list()
{
	char **result = (char**)myMalloc(sizeof(char*));
	result[0] = NULL;
	return result;
}
