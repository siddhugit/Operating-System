/*
 * miscfs.h
 *
 *  Created on: Oct 31, 2015
 *      Author: URM
 */

#ifndef MISCFS_H_
#define MISCFS_H_

void*apm_open(const char *name,char mode);
int apm_read(void *fp,char* buff);

void* ttr_open(const char *name,char mode);
int ttr_read(void *fp,char* buff);

void* tsr_open(const char *name,char mode);
int tsr_read(void *fp,char* buff);

void misc_close(void *fp);
void misc_create(const char *name);
void misc_delete(const char *name);
int misc_write(void *fp,char c);
char** misc_list();

#endif /* MISCFS_H_ */
