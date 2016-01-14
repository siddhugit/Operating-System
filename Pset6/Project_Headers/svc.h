/**
 * svc.h
 * Routines for supervisor calls
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */
#include <stdint.h>
#include "defs.h"
struct datetime;

#ifndef _SVC_H
#define _SVC_H

typedef uint32_t pid_t;

#define SVC_MaxPriority 15
#define SVC_PriorityShift 4

// Implemented SVC numbers


#define SVC_READ 0
#define SVC_WRITE 1
#define SVC_OPEN 2
#define SVC_CLOSE 3
#define SVC_CREATE 4
#define SVC_DELETE 5
#define SVC_MALLOC 6
#define SVC_FREE 7
#define SVC_PDBDEMO 8
#define SVC_SETDATETIME 9
#define SVC_GETDATETIME 10
#define SVC_PID 11
#define SVC_SPAWN 12
#define SVC_YIELD 13
#define SVC_BLOCK 14
#define SVC_WAKE 15
#define SVC_KILL 16
#define SVC_WAIT 17
#define SVC_TERMINATE 18
#define SVC_PS 19


void svcInit_SetSVCPriority(unsigned char priority);
void svcHandler(void);


int svc_open(const char *name,char mode);
void svc_close(int fd);
int svc_create(const char *name,char mode);
void svc_delete(const char *name);
int svc_read(int fd,char *buff);
int svc_write(int fd,char ch);

void *svc_malloc(unsigned int size);
void svc_free(void *ptr);

void svc_pdbDemo(int seconds);

int svc_setDateAndTime(const char* dateTimeStr);
int svc_getDateAndTime(struct datetime* dtime);

pid_t svc_pid(void);
int svc_spawn(struct spawnArgs *args);
void svc_yield(void);
void svc_block(void);
int svc_wake(pid_t targetPid);	
int svc_kill(pid_t targetPid);	
void svc_wait(pid_t targetPid);
void svc_terminate();
void svc_ps();

#endif /* ifndef _SVC_H */
