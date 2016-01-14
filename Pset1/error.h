//shell program errno definition
enum Shell_ErrNo
{ 
	NONE,	
	BAD_COMMAND_NAME = 1,
	TOO_MANY_ARGUMENTS = 2 
};
//converts error num to string 
char* my_strerr(enum Shell_ErrNo errnum);
