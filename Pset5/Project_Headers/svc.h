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

struct datetime;

#ifndef _SVC_H
#define _SVC_H

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

#endif /* ifndef _SVC_H */
