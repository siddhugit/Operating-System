#ifndef _ERROR_H__
#define _ERROR_H__

//shell program errno definition
enum Shell_ErrNo
{ 
	NONE = 0,	
	BAD_COMMAND_NAME = 1,
	TOO_MANY_ARGUMENTS = 2,
	TOO_FEW_ARGUMENTS = 3,
	NOT_VALID_ARGUMENT = 4,
	MEMORY_NOT_ALLOCATED = 5,
	DELETE_NULL = 6,
	OUT_OF_MEMROY = 7
};
//converts error num to string 
char* my_strerr(enum Shell_ErrNo errnum);

#endif //_ERROR_H__
