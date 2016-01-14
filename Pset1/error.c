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
		default:
			return "";
	}
}
