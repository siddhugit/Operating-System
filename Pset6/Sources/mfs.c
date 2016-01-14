/**
 **     This module implements a concrete file system interface for in-memory file system 
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "fstring.h"
#include "memory.h" 
#include "error.h"

#define SPECIAL_PID 256
//shell program errno declaration
extern enum Shell_ErrNo my_errno;

//maximum file size supported
static const unsigned int BLOCK_SIZE = 512;

//file block data structure
struct mfs_file
{
	char *fname;
	uint32_t size;
	unsigned char *data;
	struct mfs_file *next;
};

//file stream data structure
struct mfs_stream
{
	struct mfs_file* file;
	uint32_t offset;
	uint32_t readoffset;
};

//files are arranged in linked list, head of the list
static struct mfs_file* Head = NULL;

/*
*   Private helper function to create a new block for a new file and initialize file params.
*   Parameters:
*       o const char* name - name of file.
*       Return : pointer to new block of file data
*/
static struct mfs_file* mfs_construct(const char* name)
{
	void *fileHeaderMemory = myMalloc(sizeof(struct mfs_file));
	setMemoryPid(fileHeaderMemory,SPECIAL_PID);
	struct mfs_file* newfile = (struct mfs_file*)fileHeaderMemory;
	void * blockMemory = myMalloc(BLOCK_SIZE);
	setMemoryPid(blockMemory,SPECIAL_PID);
	newfile->data = (unsigned char *)blockMemory;
	newfile->size = 0;
	newfile->fname = new_str(name);
	return newfile;
}
/*
*   Private helper function to destroy file block when its being deleted.
*   Parameters:
*       o struct mfs_file* file - file block data structure.
*/
static void mfs_destroy(struct mfs_file* file)
{
	myFree(file->data);
	myFree(file->fname);
	myFree(file);
}
/*
*   Implements open interface
*   Parameters:
*       o const char *name - file name
*       o char mode - 'rR','wW' or 'aA' 
*   Return : stream object if successful, otherwise NULL
*/
void* mfs_open(const char *name,char mode)
{
	struct mfs_file* file = Head;
	while(file)
	{
		if(strcompare(file->fname,name))
		{
			struct mfs_stream* stream = (struct mfs_stream*)myMalloc(sizeof(struct mfs_stream));
			stream->offset = 0;
			stream->readoffset = 0;
			stream->file = file;		
			if(mode == 'a' || mode == 'A')
			{
				stream->offset = file->size;
			}
			else if(mode == 'w' || mode == 'W')
			{
				file->size = 0;
			}
			return stream;
		}
		file = file->next;
	}
	return NULL;
}
/*
*   Implements create interface. Validates file name, creates a new file block
*   if file does not exist.
*   Parameters:
*       o const char *name - file name to be created
*   sets my_errno to END_OF_FILE read at end of file.
*/
void mfs_create(const char *name)
{
	if(validateFileName(name))
	{
		struct mfs_file* file = Head;
		while(file)
		{
			if(strcompare(file->fname,name))
			{
				return;
			}
			file = file->next;
		}
		struct mfs_file* newfile = mfs_construct(name);
		newfile->next = Head;
		Head = newfile;
	}
	else
	{
		my_errno = INVALID_FILE_NAME;
	}
}
/*
*   Implements delete interface. frees up memory used by file data block and removes name for 
*   file system data structure
*   Parameters:
*       o const char *name - file name to be deleted.
*/
void mfs_delete(const char *name)
{
	struct mfs_file* file = Head;
	struct mfs_file* prev = Head;
	while(file)
	{
		if(strcompare(file->fname,name))
		{
			if(prev == file)
			{
				struct mfs_file* temp = Head->next;
				mfs_destroy(Head);
				Head = temp;
			}
			else
			{
				prev = file->next;
				mfs_destroy(Head);
			}
		}
		prev = file;
		file = file->next;
	}
}
/*
*   Implements close interface
*   Parameters:
*       o void *fp - stream to close.
*/
void mfs_close(void *fp)
{
	myFree(fp);
}
/*
*   Implements read interface. Reads next bytes on successive reads.
*   Parameters:
*       o void *fp - stream to read.
*       o char* buff - character read into buff.
*       Return : Number of bytes read
*       sets my_errno to END_OF_FILE read at end of file.
*/
int mfs_read(void *fp,char* buff)
{
	struct mfs_stream* stream = (struct mfs_stream*)fp;
	struct mfs_file* file = stream->file;
	if(stream->readoffset >= 0 && stream->readoffset < file->size)
	{
		*buff = file->data[stream->readoffset];
		++stream->readoffset;
		return 1;
	}
	if(stream->readoffset == file->size)
	{
		my_errno = END_OF_FILE;
	}
	return 0;
}
/*
*   Implements write interface. Writes to next byte position on successive writes
*   Parameters:
*       o void *fp - stream to read.
*       o char c - byte to write.
*       Return : Number of bytes written
*        sets my_errno to MAX_FILE_SIZE if more than 512 bytes are written. Files cannot extend more than 512 bytes
*/
int mfs_write(void *fp,char c)
{
	struct mfs_stream* stream = (struct mfs_stream*)fp;
	struct mfs_file* file = stream->file;
	if(stream->offset >= 0 && stream->offset < BLOCK_SIZE)
	{
		file->data[stream->offset] = c;
		++stream->offset;
		++file->size;
		return 1;
	}
	if(stream->readoffset == file->size)
	{
		my_errno = MAX_FILE_SIZE;
	}
	return 0;
}
/*
*   Implements list interface. 
*   Return : Null terminated array of file names.
*/
char** mfs_list()
{
	char **result;int i = 0;
	struct mfs_file* file = Head;
	int sz = 0;
	while(file)
	{
		++sz;
		file = file->next;
	}
	result = (char**)myMalloc((sz + 1)*sizeof(char*));
	file = Head;
	while(file)
	{
		result[i] = file->fname;
		++i;
		file = file->next;
	}
	result[sz] = NULL;
	return result;
}
