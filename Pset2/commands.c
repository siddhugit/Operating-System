/**
 **     This is command handler module. It has one public interface which
 **	takes commands and arguments to run. It encapsulates command name
 **	and function pointers data structure and definitions of the functions.
 **	Command is run by searching command name in the data structure and if
 **	found invokes that function and return appropriate success or failure
 **	status.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dateutil.h"
#include "error.h"
#include "memory.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

//command functions declarations
static int cmd_date(int argc, char *argv[]);
static int cmd_echo(int argc, char *argv[]);
static int cmd_exit(int argc, char *argv[]);
static int cmd_help(int argc, char *argv[]);

//memory commands
static int cmd_malloc(int argc, char *argv[]);
static int cmd_free(int argc, char *argv[]);
static int cmd_memorymap(int argc, char *argv[]);

//number of commands supported by the shell program
static const int NUM_COMMANDS = 7;

//command functions type
typedef int (*commandfunc_t)(int argc, char *argv[]);

//command data structure tuple
struct commandEntry 
{
	char *name;
	commandfunc_t functionp;
};

//command data structure
static struct commandEntry commands[] = {{"date", cmd_date},
			   {"echo", cmd_echo},
			   {"exit", cmd_exit},
			   {"help", cmd_help},
			   {"malloc", cmd_malloc},
			   {"free", cmd_free},
			   {"memorymap", cmd_memorymap}};


static int cmd_malloc(int argc, char *argv[])
/*
*   function definition for memory malloc command. Calls myMalloc() of memory
*   module with size of memory to be allocated passed as an argument. Prints
*	the memory location on standard output if allocated.
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   sets my_errno to TOO_MANY_ARGUMENTS if more than two argument is passed,
*	sets my_errno to if less than two arguments are passed.
*/
{
	if(argc > 2)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 2 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	char *endPtr;
	unsigned long int size = strtoul(argv[1],&endPtr,0);
	if(endPtr == argv[1] || *endPtr != '\0')
 	{
		my_errno = NOT_VALID_ARGUMENT;
		return 1;
	}
	my_errno = NONE;
	void *ptr = myMalloc(size);
	if(my_errno != NONE)
	{
		return 1;
	}
	fprintf(stdout,"%p\n",ptr);
	return 0;
}
static int cmd_free(int argc, char *argv[])
/*
*   function definition for memory free command. Calls myFree() to free
*   memory passed as an argument.
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   sets my_errno to TOO_MANY_ARGUMENTS if more than two argument is passed,
*	sets my_errno to if less than two arguments are passed.
*/
{
	if(argc > 2)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc > 2)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 2 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	char *endPtr;
	void *ptr = (void*)strtoul(argv[1],&endPtr,0);
	if(endPtr == argv[1] || *endPtr != '\0')
 	{
		my_errno = NOT_VALID_ARGUMENT;
		return 1;
	}
	my_errno = NONE;
	myFree(ptr);
	if(my_errno != NONE)
	{
		return 1;
	}
	fprintf(stdout,"Memory freed successfully\n");
	return 0;
}
static int cmd_memorymap(int argc, char *argv[])
/*
*   function definition for memory map command. Calls memmoryMap() of memory
*   module to print current state of memory.
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   sets my_errno to TOO_MANY_ARGUMENTS if more than one argument is passed
*/
{
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	memoryMap();
	return 0;
}
static int cmd_date(int argc, char *argv[])
/*
*   function definition for date command
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   sets my_errno to TOO_MANY_ARGUMENTS if more than one argument is passed
*/
{
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	print_date();//prints date on screen
	return 0;
}
static int cmd_echo(int argc, char *argv[])
/*
*   function definition for echo command
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : always 0
*/
{
	int i;
	if(argc > 1)
	{	
		fprintf(stdout,"%s",argv[1]);
		for(i = 2;i < argc;++i)
		{
			fprintf(stdout," %s",argv[i]);
		}
	}
	fprintf(stdout,"\n");
	return 0;
}
static int cmd_exit(int argc, char *argv[])
/*
*   function definition for exit command
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   It does not report error if too many arguments are passed because user
*	intends to exit anyways. 
*/
{
	exit(0);//exit anyways
}
static int cmd_help(int argc, char *argv[])
/*
*   function definition for help command
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : 0 if successful, otherwise 1
*   sets my_errno to TOO_MANY_ARGUMENTS if more than one argument is passed
*/
{
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	fprintf(stdout,"Command	     Description					Usage\n");
	fprintf(stdout,"-------	     -----------					-----\n");
	fprintf(stdout,"date	     Print the date					date\n");
	fprintf(stdout,"echo	     Echo arguments to the standard output		echo [args]\n");
	fprintf(stdout,"help	     Print brief description of the shell commands	help\n");
	fprintf(stdout,"exit	     Quit the shell					exit\n");
	return 0;
}

static int strcompare(const char *str1,const char *str2)
/*
*   Private function to compare two strings.  Comparison is 
*     case-sensitive.
*   Parameters:
*       o const char *str1- first string
*       o const char *str2- second string 
*   Return : 1 if strings are equal, otherwise 0
*/
{	
	if(str1 == NULL || str2 == NULL)
		return 0;	
	while(*str1 && *str2)
	{
		if(*str1 != *str2)//any character mismatch
			return 0;
		++str1;++str2;
	}	
	if(*str1  || *str2)//if more characters left in any of the two strings
		return 0;
	return 1;//strings are equal
}

static commandfunc_t find_command(const char* cmdName)
/*
*   Private function to search command name in the data structure. 
*     case-sensitive.
*   Parameters:
*       o const char *cmdName- command name string 
*   Return : corresponding function pointer if found, NULL otherwise
*/
{
	int i;
	for(i = 0;i < NUM_COMMANDS;++i)//iteration on data structure
	{
		if(strcompare(cmdName,commands[i].name))
			return commands[i].functionp;//command name found
	}
	return NULL;//command name not found
}

int run_command(int argc, char *argv[])
/*
*   The only public interface of this module. Searches the command name
* 	and if found executes it.
*   Parameters:
*       o int argc- number of arguments
*       o char *argv[] - array of arguments  
*   Return : exit status of command handler functions, otherwise 1
*   sets my_errno to BAD_COMMAND_NAME if unsupported command is run
*/
{
	const char* cmdName = argv[0];
	commandfunc_t func = find_command(cmdName);//search command name
	if(func != NULL)//run command if found
	{
		return func(argc,argv);
	}
	my_errno = BAD_COMMAND_NAME;//sets errno if command not found
	return 1;
}
