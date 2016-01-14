/*
 * defs.h
 *
 *  Created on: Dec 6, 2015
 *      Author: URM
 */
#include <stdint.h>

#ifndef DEFS_H_
#define DEFS_H_

typedef void (*EntryFunc)(int,char**);
typedef uint32_t pid_t;

struct spawnArgs
{
	EntryFunc funcPtr;
	int argc;
	char **argv;
	uint32_t stackSize;
	pid_t *spawnedPidptr;
	int stdIn;
	int stdOut;
};

#endif /* DEFS_H_ */
