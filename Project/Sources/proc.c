/*
 * proc.c
 *
 *  Created on: Dec 1, 2015
 *      Author: URM
 */

#include <stdint.h>
#include "derivative.h"
#include "scheduler.h"

void di()
{
	__asm("cpsid i");
}

void ei()
{
	__asm("cpsie i");
}

int spawn(struct spawnArgs *args)
{
	di();
	addNewProc(args);
	ei();
	return 0;
}

void yield(void)
{
	SCB_ICSR  |= SCB_ICSR_PENDSVSET_MASK;
}


pid_t pid(void)
{
	di();
	pid_t pid = getCurrPid();
	ei();
	return pid;
}

void block(void)
{
	di();
	setCurrBlocked();
	ei();
	yield();
}
void wait(pid_t targetPid)
{
	di();
	int isNotTerminatd = addToWaitingPidList(targetPid);
	if(isNotTerminatd)
	{
		setCurrBlocked();
		ei();
		yield();
	}
	else
	{
		ei();
	}
}
int kill(pid_t targetPid)
{
	di();
	setPidTerminated(targetPid);
	ei();
	return 0;
}

int wake(pid_t targetPid)
{
	di();
	setPidReady(targetPid);
	ei();
	return 0;
}	
	
int getStdFileIn()
{
	di();
	int fd = getCurrStdIn();
	ei();
	return fd;
}
int getStdFileOut()
{
	di();
	int fd = getCurrStdOut();
	ei();
	return fd;
}











