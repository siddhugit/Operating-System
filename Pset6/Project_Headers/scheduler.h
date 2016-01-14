/*
 * scheduler.h
 *
 *  Created on: Dec 1, 2015
 *      Author: URM
 */
#include "defs.h"

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

typedef uint32_t pid_t;

void addNewProc(struct spawnArgs *args);
uint32_t getNextProcSP();
void setCurrProcSP(uint32_t sp,uint32_t cpuTime);
pid_t getCurrPid();
void setCurrBlocked();
void setCurrTerminated();
void setPidReady(pid_t pid);
void setPidTerminated(pid_t pid);
int addToWaitingPidList(pid_t pid);
void addOpenFd(pid_t pid,int fd);
void removeOpenFd(pid_t pid,int fd);
void ps_list();

#endif /* SCHEDULER_H_ */
