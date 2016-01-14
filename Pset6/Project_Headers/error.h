#ifndef _ERROR_H__
#define _ERROR_H__

//shell program errno definition
enum Shell_ErrNo
{ 
	NONE_ERROR = 0,	
	BAD_COMMAND_NAME = 1,
	TOO_MANY_ARGUMENTS = 2,
	TOO_FEW_ARGUMENTS = 3,
	NOT_VALID_ARGUMENT = 4,
	MEMORY_NOT_ALLOCATED = 5,
	DELETE_NULL = 6,
	OUT_OF_MEMROY = 7,
	INVALID_FILE_NAME = 8,
	NOT_A_LED = 9,
	NOT_A_BTN = 10,
	END_OF_FILE = 11,
	OPEN_WRITE_ERROR = 12,
	OPEN_READ_ERROR = 13,
	INVALID_MODE = 14,
	MAX_FILE_SIZE = 15,
	FD_NOT_OPEN = 16,
	INVALID_DATE_FORMAT = 17
};
//converts error num to string 
char* my_strerr(enum Shell_ErrNo errnum);

#endif //_ERROR_H__
