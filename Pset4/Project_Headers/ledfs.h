//VFS interfacing with concrete led file system

#ifndef _LEDFS_H__
#define _LEDFS_H__

void* led_open(const char *name,char mode);
void led_close(void *fp);
void led_create(const char *name);
void led_delete(const char *name);
int led_read(void *fp,char* buff);
int led_write(void *fp,char c);
char** led_list();

#endif //_LEDFS_H__
