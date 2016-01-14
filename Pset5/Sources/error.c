/**
 **     Contains error handling definitions for the shell program.
 **/

#include "error.h"

//default shell program errno
enum Shell_ErrNo my_errno = NONE_ERROR;

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
		case INVALID_FILE_NAME:
			return "Not a valid file name";
		case NOT_A_LED:
			return "Not a led";
		case NOT_A_BTN:
			return "Not a button";
		case END_OF_FILE:
			return "End of file";
		case OPEN_READ_ERROR:
			return "File is open for read";
		case OPEN_WRITE_ERROR:
			return "Multiple open on write not allowed";
		case INVALID_MODE:
					return "Invalid mode";
		case MAX_FILE_SIZE:
			return "File cannot be extended";
		case FD_NOT_OPEN:
			return "File descriptor not open";
		case INVALID_DATE_FORMAT:
					return "Valid date format is: MM/dd/yy HH:mm:ss.zzz";
		default:
			return "";
	}
}
