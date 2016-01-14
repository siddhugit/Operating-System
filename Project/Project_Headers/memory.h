#ifndef _MEMORY_H__
#define _MEMORY_H__

#define SPECIAL_PID 256

typedef uint32_t pid_t;

void init_memory();
void *myMalloc(unsigned int size);
void myFree(void *ptr);
void memoryMap();
void setMemoryPid(void *ptr,pid_t pid);
void systemFree(void *ptr);
void garbageCollection(pid_t pid);

#endif //_MEMORY_H__
