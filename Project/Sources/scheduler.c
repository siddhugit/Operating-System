/*
 * scheduler.c
 *
 *  Created on: Dec 1, 2015
 *      Author: URM
 */

#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include "derivative.h"
#include "memory.h"
#include "scheduler.h"
#include "proc.h"
#include "vfs.h"
#include "svc.h"
#include "ustream.h"

enum PROCESS_STATE{READY,BLOCKED,RUNNING,TERMINATED};

#define INITIAL_STACK_FRAME_SIZE 23
#define XPSR_MASK 0x1000000u // | 0x200u
#define MAGIC_NUMBER 0xFFFFFFF9
#define NUM_PIDS 32
#define NUM_FDS 32
#define QUANTUM_VAL_MS  120000.0


struct PCB
{
	uint32_t pid;
	uint32_t sp;
	uint32_t stackMemory;
	uint32_t stackSize;
	double cpuTime;
	uint32_t waitingPids[NUM_PIDS];
	int openFds[NUM_FDS];
	enum PROCESS_STATE state;
	int std_in;
	int std_out;
	struct PCB *next;
	struct PCB *prev;
};

struct PCB *CURR_PCB = NULL;


static int PIDS[NUM_PIDS] = {0};

static int getNewPID()
{
	int i = 0;	
	for(;i < NUM_PIDS; ++i)
	{
		if(PIDS[i] == 0)
		{
			PIDS[i] = 1;
			return i;
		}
	}
	return -1;
}

static void addToList(struct PCB* pcb)
{
	if (CURR_PCB == NULL)
	{
		CURR_PCB = pcb;
		CURR_PCB->next = CURR_PCB;
		CURR_PCB->prev = CURR_PCB;
	}
	else if(CURR_PCB->next == CURR_PCB)
	{
		pcb->next = CURR_PCB;
		pcb->prev = CURR_PCB;
		CURR_PCB->next = pcb;
		CURR_PCB->prev = pcb;
	}
	else
	{
		struct PCB* last = CURR_PCB->prev;
		last->next = pcb;
		pcb->next = CURR_PCB;
		pcb->prev = last;
		CURR_PCB->prev = pcb;
	}
}

static void deleteFromList(struct PCB* pcb)
{
	if (CURR_PCB == NULL)
	{
	}
	else if(CURR_PCB->next == CURR_PCB)
	{
		CURR_PCB = NULL;
	}
	else
	{
		struct PCB* first = pcb->next;
		struct PCB* last = pcb->prev;
		last->next = first;
		first->prev = last;
		systemFree(pcb);
	}
}

static struct PCB* getPCBFromPID(uint32_t pid)
{
	struct PCB* curr = CURR_PCB;
	if (curr == NULL)
	{
		return NULL;
	}
	else if(curr->pid == pid)
	{
		return curr;
	}
	else
	{
		struct PCB* next = curr->next;
		while(next != curr)
		{
			if(next->pid == pid)
			{
				//found
				return next;
			}
			next = next->next;
		}
	}
	return NULL;
}

static void cleanUp(struct PCB* pcb)
{
	//release pid number
	PIDS[pcb->pid] = 0;
	//wake waiting pids
	int i = 0;
	while(pcb->waitingPids[i] != NUM_PIDS)
	{
		struct PCB* waitingPcb = getPCBFromPID(pcb->waitingPids[i]);
		waitingPcb->state = READY;
		++i;
	}
	//close open file descriptors
	for(i = 0;i < NUM_FDS; ++i)
	{
		if(pcb->openFds[i])
		{
			vfs_close(i);
		}
	}
	//delete stack
	systemFree((void*)pcb->stackMemory);
	//do a garbage collection
	garbageCollection(pcb->pid);
	//free pc block memory
	deleteFromList(pcb);
	
}

int addToWaitingPidList(pid_t pid)
{
	struct PCB* pcb = getPCBFromPID(pid);
	if(pcb && pcb->state != TERMINATED)
	{
		int i = 0;
		for(; i < NUM_PIDS - 1; ++i)
		{
			if(pcb->waitingPids[i] == NUM_PIDS)
			{
				pcb->waitingPids[i] = CURR_PCB->pid;
				pcb->waitingPids[i+1] = NUM_PIDS;
				break;
			}
		}
		return 1;
	}
	return 0;
}

static struct PCB* getNextFromList()
{
	if (CURR_PCB == NULL)
	{
		return NULL;
	}
	else if(CURR_PCB->next == CURR_PCB)
	{
		if(CURR_PCB->state == READY)
		{
			return CURR_PCB;
		}
	}
	else
	{
		struct PCB* next = CURR_PCB->next;
		//while(next != CURR_PCB)
		while(1)
		{
			if(next->state == READY)
			{
				//found
				CURR_PCB = next;
				return next;
			}
			else if(next->state == TERMINATED)
			{
				struct PCB* temp = next;
				next = temp->next;
				cleanUp(temp);
			}
			else
			{
				next = next->next;
			}
		}
	}
	return NULL;
}


static void setProcState(enum PROCESS_STATE state)
{
	CURR_PCB->state = state;
}

//gets memory with highest address ( malloc + 4096 )
static uint32_t* initStack(uint32_t* stack,EntryFunc funcptr,int argc,char *argv[])
{
	int i = 0;
	stack -= INITIAL_STACK_FRAME_SIZE;
	for(;i < INITIAL_STACK_FRAME_SIZE; ++i)
	{
		stack[i] = 0;
	}
	stack[15] = (uint32_t)argc;
	stack[16] = (uint32_t)argv;
	stack[4] = (uint32_t)(stack + 9);
	stack[14] = MAGIC_NUMBER;
	stack[20] = (uint32_t)svc_terminate;
	stack[21] = (uint32_t)funcptr;
	stack[22] |= XPSR_MASK; 
	return stack;
}



void addNewProc(struct spawnArgs *args)
{
	struct PCB* pcb = NULL;
	int pid = getNewPID();
	int i,j;
	uint32_t stackSize = args->stackSize;
	EntryFunc funcptr = args->funcPtr;
	if(pid != -1)
	{
		uint32_t *stack = (uint32_t *)myMalloc(stackSize);
		if(stack != NULL)
		{
			int sz = (stackSize/sizeof(uint32_t));
			uint32_t* sp = initStack(stack + sz,funcptr,args->argc,args->argv);
			pcb =  (struct PCB *)myMalloc(sizeof(struct PCB));
			setMemoryPid(pcb,SPECIAL_PID);
			pcb->pid = pid;
			setMemoryPid((void*)stack,pid);
			if(args->argv)
			{
				for(j = 0;j < args->argc;++j)
				{
					setMemoryPid((void*)(args->argv[j]),pid);
				}
				setMemoryPid((void*)(args->argv),pid);
			}
			pcb->sp = (uint32_t)sp;
			pcb->stackMemory = (uint32_t)stack;
			pcb->stackSize = stackSize;
			pcb->std_in = args->stdIn;
			pcb->std_out = args->stdOut;
			pcb->cpuTime = 0;
			pcb->state = READY;
			pcb->waitingPids[0] = NUM_PIDS;
			for(i = 0;i < NUM_FDS; ++i)
			{
				pcb->openFds[i] = 0;
			}
			pcb->openFds[pcb->std_in] = 1;
			pcb->openFds[pcb->std_out] = 1;
			*(args->spawnedPidptr) = pid;
		}
	}
	//do list manipulation
	if(pcb != NULL)
	{
		addToList(pcb);
	}
	
}

uint32_t getNextProcSP()
{
	di();
	struct PCB* pcb = getNextFromList();
	ei();
	if(pcb != NULL)
	{
		pcb->state = RUNNING;
		return pcb->sp;
	}
	return 0;
}

void setCurrProcSP(uint32_t sp,uint32_t cpuTime)
{
	di();
	if(CURR_PCB->state == RUNNING)
	{
		CURR_PCB->state = READY;
	}
	CURR_PCB->sp = sp;
	double fCpuTime = (double)cpuTime;
	fCpuTime /= QUANTUM_VAL_MS;
	CURR_PCB->cpuTime += fCpuTime;
	ei();
}

pid_t getCurrPid()
{
	return CURR_PCB->pid;
}

int getCurrStdOut()
{
	return CURR_PCB->std_out;
}

int getCurrStdIn()
{
	return CURR_PCB->std_in;
}

void setCurrBlocked()
{
	setProcState(BLOCKED);
}

void setCurrTerminated()
{
	setProcState(TERMINATED);
	yield();
}

void setPidTerminated(pid_t pid)
{
	struct PCB* pcb = getPCBFromPID(pid);
	pcb->state = TERMINATED;
	if(getCurrPid() == pid)
	{
		yield();
	}
}

void setPidReady(pid_t pid)
{
	struct PCB* pcb = getPCBFromPID(pid);
	pcb->state = READY;
	if(getCurrPid() == pid)
	{
		yield();
	}
}

void addOpenFd(pid_t pid,int fd)
{
	struct PCB* pcb = getPCBFromPID(pid);
	pcb->openFds[fd] = 1;
}

void removeOpenFd(pid_t pid,int fd)
{
	struct PCB* pcb = getPCBFromPID(pid);
	pcb->openFds[fd] = 0;
}

static char* getSateStr(enum PROCESS_STATE state)
{
	switch(state)
	{
		case READY:
			return "Ready";
		case RUNNING:
			return "Running";
		case BLOCKED:
			return "Blocked";
		case TERMINATED:
			return "Terminated";
		default:
			return "";
	}
}

void ps_list()
{
	char tempstr[1024];
	di();
	struct PCB* curr = CURR_PCB;
	if (curr == NULL)
	{
		return;
	}
	sprintf(tempstr,"pid	     state		CPU Time (in millisecs)\r\n");uprintf(tempstr);
	sprintf(tempstr,"---	     -----		-----------------------\r\n");uprintf(tempstr);
	int ctime = curr->cpuTime;
	sprintf(tempstr,"%lu	     %s		%d\r\n",curr->pid,getSateStr(curr->state),ctime);uprintf(tempstr);
	struct PCB* next = curr->next;
	while(next != curr)
	{
		ctime = next->cpuTime;
		sprintf(tempstr,"%lu	     %s		%d\r\n",next->pid,getSateStr(next->state),ctime);uprintf(tempstr);
		next = next->next;
	}
	ei();
}

