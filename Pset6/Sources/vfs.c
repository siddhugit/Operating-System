/**
 **     This module implements interface for user's application in VFS.
 **     It calls concrete file systems implementation to perform device specific tasks.
 **     This module is essentially VFS abstraction. 
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vfs.h"
#include "fstring.h"
#include "memory.h" 
#include "mfs.h"
#include "ledfs.h"
#include "btnfs.h"
#include "uartfs.h"
#include "lcdfs.h"
#include "miscfs.h"
#include "error.h"
#include "ustream.h"
#include "scheduler.h"
#include "proc.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

//Maximum number of open file descriptors supported
#define NUM_FDS 64
//Number of devices supported currently, just realized, will use sizeof next time to make it extensible
#define NUM_DEVICES 8

//File open modes
enum MODE{READ,WRITE,APPEND,NONE};
//Concrete file system types
enum MAJOR{LED=0,BTN=1,MFS=2,LCD=3,UART=4,APM=5,TTR=6,TSR=7,NAD=8};

//interface for concrete file systems to implememnt
typedef void* (*iopen)(const char *name,char mode);
typedef void (*iclose)(void *fp);
typedef void (*icreate)(const char *name);
typedef void (*idelete)(const char *name);
typedef int (*iread)(void *fp,char* buff);
typedef int (*iwrite)(void *fp,char c);
typedef char** (*ilist)();

//device (concrete file systems) interface class
struct device
{
	iopen open_func;
	iclose close_func;
	icreate create_func;
	idelete delete_func;
	iread read_func;
	iwrite write_func;
	ilist list_func;
};
//concrete file system factory
static struct device devices[NUM_DEVICES] = {
				{led_open,led_close,
				led_create,led_delete,
				led_read,led_write,led_list},
				
				{btn_open,btn_close,
				btn_create,btn_delete,
				btn_read,btn_write,btn_list},
				
				{mfs_open,mfs_close,
				mfs_create,mfs_delete,
				mfs_read,mfs_write,mfs_list},
				
				{lcd_open,lcd_close,
				lcd_create,lcd_delete,
				lcd_read,lcd_write,lcd_list},
								
				{uart_open,uart_close,
				uart_create,uart_delete,
				uart_read,uart_write,uart_list},
				
				{apm_open,misc_close,
				misc_create,misc_delete,
				apm_read,misc_write,misc_list},
								
				{ttr_open,misc_close,
				misc_create,misc_delete,
				ttr_read,misc_write,misc_list},
				
				{tsr_open,misc_close,
				misc_create,misc_delete,
				tsr_read,misc_write,misc_list}
		    	};

static const char* LED_PATH = "/led/";
static const char* BTN_PATH = "/btn/";
static const char* LCD_PATH = "/lcd";
static const char* APM_PATH = "/apm";
static const char* TTR_PATH = "/ttr";
static const char* TSR_PATH = "/tsr/";
static const char* UART_PATH = "/uart";
//Open file descriptors data structure
struct open_fd
{
	enum MAJOR major;
	char *name;
	enum MODE mode;
	void *stream;
};

//file descriptor array
static struct open_fd OPEN_FDS[NUM_FDS];

//helper function to covert char mode to enum type
static enum MODE getMode(char mode)
{
	switch(mode)
	{
		case 'r':
		case 'R':
			return READ;
		case 'w':
		case 'W':
			return WRITE;
		case 'a':
		case 'A':
			return APPEND;
		default:
			return NONE;
	}
}
/*
*   Private helper function to check if file is aready open.
*   Parameters:
*       o const char* name - name of file.
*       Return : true if open, otherwise false
*/
static bool isOpen(const char* name)
{
	int i;
	for(i = 0; i < NUM_FDS; ++i)
	{
		if(OPEN_FDS[i].stream != NULL)
		{
			if(strcompare(OPEN_FDS[i].name,name))
				return true;
		}
	}
	return false;
}
/*
*   Private helper function to check if file is already open in write mode.
*   Parameters:
*       o const char* name - name of file.
*       Return : true if open in wite mode, otherwise false
*/
static bool isOpenInWrite(const char* name)
{
	int i;
	for(i = 0; i < NUM_FDS; ++i)
	{
		if(OPEN_FDS[i].stream != NULL)
		{
			if(strcompare(OPEN_FDS[i].name,name) && (OPEN_FDS[i].mode == WRITE || OPEN_FDS[i].mode == APPEND))
				return true;
		}
	}
	return false;
}
/*
*   Private helper function to create and initialize file data structures in VFS.
*   Parameters:
*       o const char* name - name of file.
*       o char mode - open mode.
*       Return : file descriptor at which file is opened.
*/
static int createFile(const char *name,int fd,char mode)
{
	enum MAJOR major = NAD;	
	if(startsWith(name,LED_PATH))
	{
		major = LED;
	}
	else if(startsWith(name,BTN_PATH))
	{
		major = BTN;
	}
	else
	{
		major = MFS;
	}
	if(major != NAD)
	{
		devices[major].create_func(name);
		return vfs_open(name,mode);
	}
	return -1;
}
/*
*   Private helper function to open and initialize file data structures in VFS.
*   Parameters:
*       o const char* name - name of file.
*       o char mode - open mode.
*       Return : file descriptor at which file is opened.
*/
static int openFile(const char *name,int fd,char mode)
{
	char *fname;
	void *stream;
	enum MAJOR major = NAD;
	enum MODE imode = getMode(mode);	
	if(startsWith(name,LED_PATH))
	{
		fname = new_str(name + str_len(LED_PATH));
		major = LED;
	}
	else if(startsWith(name,BTN_PATH))
	{
		fname = new_str(name + str_len(BTN_PATH));
		major = BTN;
	}
	else if(startsWith(name,LCD_PATH))
	{
		fname = new_str(name);
		major = LCD;
	}
	else if(startsWith(name,UART_PATH))
	{
		fname = new_str(name);
		major = UART;
	}
	else if(startsWith(name,APM_PATH))
	{
		fname = new_str(name);
		major = APM;
	}
	else if(startsWith(name,TTR_PATH))
	{
		fname = new_str(name);
		major = TTR;
	}
	else if(startsWith(name,TSR_PATH))
	{
		fname = new_str(name + str_len(TSR_PATH));
		major = TSR;
	}
	else
	{
		fname = new_str(name);
		major = MFS;
	}
	if(major != NAD)
	{
		stream = devices[major].open_func(fname,mode);
		if(stream)
		{
			OPEN_FDS[fd].stream = stream;
			OPEN_FDS[fd].major = major;
			OPEN_FDS[fd].name = fname;
			OPEN_FDS[fd].mode = imode;
			return fd;
		}
	}
	return -1;
}
/*
*   Implements user's application open interface. Calls concrete file system's open
*   after initial validation.
*   Parameters:
*       o const char *name - file name
*       o char mode - 'rR','wW' or 'aA' 
*   Return : file descriptor at which file is opened.
*   sets my_errno to OPEN_WRITE_ERROR if opened in write mode and file is already open.
*   sets my_errno to OPEN_READ_ERROR if opened and file is already open in write mode.
*   sets my_errno to INVALID_MODE if mode is not supported.
*/
int vfs_open(const char *name,char mode)
{
	enum MODE imode = getMode(mode);
	if((imode == WRITE || imode == APPEND) && isOpen(name))
	{
		my_errno = OPEN_WRITE_ERROR;
		return -1;
	}
	if((imode == READ) && isOpenInWrite(name))
	{
		my_errno = OPEN_READ_ERROR;
		return -1;
	}
	if(imode != READ && imode != WRITE && imode != APPEND)
	{
		my_errno = INVALID_MODE;
		return -1;
	}
		
	int i;
	for(i = 0; i < NUM_FDS; ++i)
	{
		if(OPEN_FDS[i].stream == NULL)
		{
			int openFd = openFile(name,i,mode);
			addOpenFd(pid(),openFd);
			return openFd;
		}
	}	
	return -1;
}
/*
*   Implements user's application close interface.Calls concrete file system's close
*   after initial validation.
*   Parameters:
*       o int fd - file descriptor to close.
*/
void vfs_close(int fd)
{
	if(fd < 0 || fd >= NUM_FDS)
	{
		return;
	}
	if(OPEN_FDS[fd].stream != NULL)
	{
		enum MAJOR major = OPEN_FDS[fd].major;
		devices[major].close_func(OPEN_FDS[fd].stream);
	}
	myFree(OPEN_FDS[fd].name);
	OPEN_FDS[fd].stream = NULL;
	removeOpenFd(pid(),fd);
}
/*
*   Implements user's application create interface. Calls concrete file system's create
*   after initial validation.
*   Parameters:
*       o const char *name - file name to be created
*   sets my_errno to END_OF_FILE read at end of file.
*/
int vfs_create(const char *name,char mode)
{
	enum MODE imode = getMode(mode);
	if((imode == WRITE || imode == APPEND) && isOpen(name))
	{
		return -1;
	}
	if((imode == READ) && isOpenInWrite(name))
	{
		return -1;
	}
	int i;
	for(i = 0; i < NUM_FDS; ++i)
	{
		if(OPEN_FDS[i].stream == NULL)
		{
			return createFile(name,i,mode);
		}
	}	
	return -1;
}
/*
*   Implements delete interface. frees up VFS resources and Calls concrete file system's delete.
*   Parameters:
*       o const char *name - file name to be deleted.
*/
void vfs_delete(const char *name)
{
	char *fname;
	enum MAJOR major = NAD;
	if(isOpen(name))
	{
		return;
	}
	if(startsWith(name,LED_PATH))
	{
		fname = new_str(name + str_len(LED_PATH));
		major = LED;
	}
	else if(startsWith(name,BTN_PATH))
	{
		fname = new_str(name + str_len(BTN_PATH));
		major = BTN;
	}
	else
	{
		fname = new_str(name);
		major = MFS;
	}
	devices[major].delete_func(fname);
	myFree(fname);
}
/*
*   Implements user's application read interface. fetches concrete system's stream
*   object and calls it's read.
*   Parameters:
*       o int fd - file descriptor to read.
*       o char* buff - character read into buff.
*       Return : Number of bytes read
*       sets my_errno to FD_NOT_OPEN if file descriptor is not valid.
*/
int vfs_read(int fd,char *buff)
{
	if(fd < 0 || fd >= NUM_FDS)
	{
		my_errno = FD_NOT_OPEN;
		return 0;
	}
	if(OPEN_FDS[fd].stream != NULL)
	{	
		enum MAJOR major = OPEN_FDS[fd].major;
		return devices[major].read_func(OPEN_FDS[fd].stream,buff);
	}
	my_errno = FD_NOT_OPEN;
	return 0;
}
/*
*   Implements user's application write interface. fetches concrete system's stream
*   object and calls it's write.
*   Parameters:
*       o int fd - file descriptor to read.
*       o char ch - byte to write.
*       Return : Number of bytes written
*       sets my_errno to FD_NOT_OPEN if file descriptor is not valid.
*/
int vfs_write(int fd,char ch)
{
	if(fd < 0 || fd >= NUM_FDS)
	{
		my_errno = FD_NOT_OPEN;
		return 0;
	}
	if(OPEN_FDS[fd].stream != NULL)
	{
		if(OPEN_FDS[fd].mode == READ)
		{
			my_errno = OPEN_READ_ERROR;
			return 0;
		}
		enum MAJOR major = OPEN_FDS[fd].major;
		return devices[major].write_func(OPEN_FDS[fd].stream,ch);
	}
	my_errno = FD_NOT_OPEN;
	return 0;
}
//bootstrap function for vfs
void vfs_init()
{
	int i;
	for(i = 0; i < NUM_FDS; ++i)
	{
		OPEN_FDS[i].stream = NULL;
	}
}
//Prints names of all the files that exist in VFS on stdout
void vfs_list()
{
	char tempstr[1024];
	char **result;
	int i,j;
	for(i = 0; i < NUM_DEVICES; ++i)
	{
		result = devices[i].list_func();
		j = 0;
		while(result[j])
		{
			sprintf(tempstr,"%s\r\n",result[j]);uprintf(tempstr);
			++j;
		}
		myFree(result);
	}
}
