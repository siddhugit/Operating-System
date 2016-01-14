/**
 **     This module implements a concrete file system interface for buttons 
 **/

#include <stdlib.h>
#include "fstring.h"
#include "btnfs.h"
#include "memory.h"
#include "pushbutton.h"
#include "error.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

enum BTN_MINOR{ONE=0,TWO=1,NBTN};

static const char* ONE_BTN = "sw1";
static const char* TWO_BTN = "sw2";

static const char* BTN_PATHS[] = {
		"/btn/sw1",
		"/btn/sw2"	
};

#define NUM_BTNS (sizeof(BTN_PATHS) / (sizeof(BTN_PATHS[0])))

//Button stream class
struct btn_stream
{
	enum BTN_MINOR minor;
};

/*
*   Implements open interface
*   Parameters:
*       o const char *name - button name
*       o char mode - 'r','w' or 'a' 
*   Return : stream object if successful, otherwise NULL
*   sets my_errno to NOT_A_BTN if button name is invalid.
*/
void* btn_open(const char *name,char mode)
{
	enum BTN_MINOR minor = NBTN;
	if(strcompare(ONE_BTN,name))
	{
		minor = ONE;
	}
	else if(strcompare(TWO_BTN,name))
	{
		minor = TWO;
	}
	else
	{
		my_errno = NOT_A_BTN;
		return NULL;
	}
	struct btn_stream *stream = myMalloc(sizeof(struct btn_stream));
	stream->minor = minor;
	return stream;
}
/*
*   Implements close interface
*   Parameters:
*       o void *fp - stream to close.
*/
void btn_close(void *fp)
{
	myFree(fp);
}
/*
*   Implements create interface. Does nothing because button already created in bootstrap
*/
void btn_create(const char *name)
{
}
/*
*   Implements create interface. Does nothing because button exists throughout application
*/
void btn_delete(const char *name)
{
}
/*
*   Implements read interface.
*   Parameters:
*       o void *fp - stream to read.
*       o char* buff - character read into buff.
*       Return : Number of bytes read
*/
int btn_read(void *fp,char* buff)
{
	struct btn_stream* stream = (struct btn_stream*)fp;
	int result = 0;
	switch(stream->minor)
	{
		case ONE:
			result = sw1In();				
			break;
		case TWO:
			result = sw2In();				
			break;
		default:
			return 0;
	}
	*buff = (result > 0) ? '1' : '0';
	return 1;
}
/*
*   Implements write interface. Does nothing as there is nothing to write.
*/
int btn_write(void *fp,char c)
{
	return 1;
}
/*
*   Implements list interface. 
*   Return : Null terminated array of button names.
*/
char** btn_list()
{
	char **result;int i = 0;
	int sz = NUM_BTNS;
	result = (char**)myMalloc((sz + 1)*sizeof(char*));
	for(i = 0;i < sz;++i)
	{
		result[i] = BTN_PATHS[i];
	}
	result[sz] = NULL;
	return result;
}
