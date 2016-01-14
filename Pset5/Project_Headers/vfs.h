//User's application interfacing with VFS

#ifndef _VFS_H__
#define _VFS_H__

void vfs_init();
int vfs_open(const char *name,char mode);
void vfs_close(int fd);
int vfs_create(const char *name,char mode);
void vfs_delete(const char *name);
int vfs_read(int fd,char *buff);
int vfs_write(int fd,char ch);
void vfs_list();

#endif //_VFS_H__
