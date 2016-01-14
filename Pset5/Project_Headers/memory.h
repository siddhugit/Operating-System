#ifndef _MEMORY_H__
#define _MEMORY_H__

void init_memory();
void *myMalloc(unsigned int size);
void myFree(void *ptr);
void memoryMap();

#endif //_MEMORY_H__
