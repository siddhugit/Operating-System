/**
 **     Contains error handling definitions for the shell program.
 **/

#include "error.h"

//default shell program errno
enum Shell_ErrNo my_errno = NONE;

char* my_strerr(enum Shell_ErrNo errnum)
/*
*   converts error number to error description string
*   Parameters:
*       o enum Shell_ErrNo errnum - error number 
*   Return : error description string
*/
{
	switch(errnum)
	{
		case BAD_COMMAND_NAME:
			return "Command Not Found";
		case TOO_MANY_ARGUMENTS:
			return "Too many arguments";
		case TOO_FEW_ARGUMENTS:
			return "Command requires one or more arguments";
		case NOT_VALID_ARGUMENT:
			return "Not a valid argument";
		case MEMORY_NOT_ALLOCATED:
			return "Not a previously allocated memory";
		case DELETE_NULL:
			return "Deallocating null pointer is no-op";
		case OUT_OF_MEMROY:
			return "Memory not available";
		default:
			return "";
	}
}
