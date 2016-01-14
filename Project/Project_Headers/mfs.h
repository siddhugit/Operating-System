//VFS interfacing with concrete in-memory file system

#ifndef _MFS_H__
#define _MFS_H__

void* mfs_open(const char *name,char mode);
void mfs_close(void *fp);
void mfs_create(const char *name);
void mfs_delete(const char *name);
int mfs_read(void *fp,char* buff);
int mfs_write(void *fp,char c);
char** mfs_list();

#endif //_MFS_H__
