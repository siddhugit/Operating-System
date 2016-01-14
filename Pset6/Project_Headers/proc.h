/*
 * proc.h
 *
 *  Created on: Dec 2, 2015
 *      Author: URM
 */

#include "defs.h"

#ifndef PROC_H_
#define PROC_H_

pid_t pid(void);
int spawn(struct spawnArgs *args);
void yield(void);
void block(void);
int wake(pid_t targetPid);	
int kill(pid_t targetPid);	
void wait(pid_t targetPid);
void di();
void ei();

#endif /* PROC_H_ */
