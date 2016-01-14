/*
 * pipefs.h
 *
 *  Created on: Dec 13, 2015
 *      Author: URM
 */

#ifndef PIPEFS_H_
#define PIPEFS_H_

void* pipe_open(const char *name,char mode);
void pipe_close(void *fp);
void pipe_create(const char *name);
void pipe_delete(const char *name);
int pipe_read(void *fp,char* buff);
int pipe_write(void *fp,char c);
char** pipe_list();

#endif /* PIPEFS_H_ */
