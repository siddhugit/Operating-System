//VFS interfacing with concrete UART file system

#ifndef _UARTFS_H__
#define _UARTFS_H__

void* uart_open(const char *name,char mode);
void uart_close(void *fp);
void uart_create(const char *name);
void uart_delete(const char *name);
int uart_read(void *fp,char* buff);
int uart_write(void *fp,char c);
char** uart_list();

#endif //_UARTFS_H__
