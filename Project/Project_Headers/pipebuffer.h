/*
 * pipebuffer.h
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 */

#ifndef PIPEBUFFER_H_
#define PIPEBUFFER_H_

/*
 * pipebuffer.c
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 */

struct PipeBuffer
{
    char *data;
    int start;
    int end;
	int size;
	int refCount;
};

struct PipeBuffer *pipeBufferCreate();
void pipeBufferDestroy(struct PipeBuffer *pb);
int pipeBufferGetChar(struct PipeBuffer *pb,char *buff);
int pipeBufferPutChar(struct PipeBuffer *pb,char ch);


#endif /* PIPEBUFFER_H_ */
