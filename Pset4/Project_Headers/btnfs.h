//VFS interfacing with concrete button file system

#ifndef _BTNFS_H__
#define _BTNFS_H__

void* btn_open(const char *name,char mode);
void btn_close(void *fp);
void btn_create(const char *name);
void btn_delete(const char *name);
int btn_read(void *fp,char* buff);
int btn_write(void *fp,char c);
char** btn_list();

#endif //_BTNFS_H__
