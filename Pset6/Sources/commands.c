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
#include "error.h"
#include "memory.h"
#include "vfs.h"
#include "svc.h"
#include "fstring.h"
#include "ustream.h"
#include "pushbutton.h"
#include "dateutil.h"
#include "PDB.h"
#include "userproc.h"
#include "scheduler.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

//command functions declarations
static int cmd_date(int argc, char *argv[]);
static int cmd_setDate(int argc, char *argv[]);
static int cmd_pdbDemo(int argc, char *argv[]);
static int cmd_echo(int argc, char *argv[]);
static int cmd_exit(int argc, char *argv[]);
static int cmd_help(int argc, char *argv[]);

//memory commands
static int cmd_malloc(int argc, char *argv[]);
static int cmd_free(int argc, char *argv[]);
static int cmd_memorymap(int argc, char *argv[]);

//file system commands
static int cmd_fopen(int argc, char *argv[]);
static int cmd_fclose(int argc, char *argv[]);
static int cmd_fgetc(int argc, char *argv[]);
static int cmd_fputc(int argc, char *argv[]);
static int cmd_create(int argc, char *argv[]);
static int cmd_delete(int argc, char *argv[]);
static int cmd_list(int argc, char *argv[]);

//I/O Commands
static int cmd_ser2lcd(int argc, char *argv[]);
static int cmd_touch2led(int argc, char *argv[]);
static int cmd_pot2ser(int argc, char *argv[]);
static int cmd_therm2ser(int argc, char *argv[]);
static int cmd_pb2led(int argc, char *argv[]);

static int cmd_forkdemo(int argc, char *argv[]);
static int cmd_ps(int argc, char *argv[]);

//command functions type
typedef int (*commandfunc_t)(int argc, char *argv[]);

#define CHAR_EOF 4

#define STACK_SIZE 4096
//command data structure tuple
struct commandEntry 
{
	char *name;
	commandfunc_t functionp;
};

//command data structure
static struct commandEntry commands[] = {{"date", cmd_date},
			   {"setdate", cmd_setDate},
			   {"toled", cmd_pdbDemo},   
			   {"echo", cmd_echo},
			   {"exit", cmd_exit},
			   {"help", cmd_help},
			   {"malloc", cmd_malloc},
			   {"free", cmd_free},
			   {"memorymap", cmd_memorymap},
			   {"fopen", cmd_fopen},
			   {"fclose", cmd_fclose},
			   {"create", cmd_create},
			   {"delete", cmd_delete},
			   {"fgetc", cmd_fgetc},
			   {"fputc", cmd_fputc},
			   {"list", cmd_list},
			   {"pb2led", cmd_pb2led},
			   {"ser2lcd", cmd_ser2lcd},
			   {"touch2led", cmd_touch2led},
			   {"pot2ser", cmd_pot2ser},
			   {"forkdemo", cmd_forkdemo},
			   {"ps", cmd_ps},
			   {"therm2ser", cmd_therm2ser}};

//number of commands supported by the shell program
#define NUM_COMMANDS (sizeof(commands) / (sizeof(struct commandEntry)))

static int cmd_ser2lcd(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	int wfd = svc_open("/lcd",'w');
	if(wfd > 0)
	{
		char buff;
		while((buff = ugetc()) != CHAR_EOF)
		{
			svc_write(wfd,buff);
		}
		svc_close(wfd);
		return 0;
	}
	return 1;
}
static int cmd_touch2led(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	int rfd[4];
	int wfd[4];
	rfd[0] = svc_open("/tsr/orange",'r');
	rfd[1] = svc_open("/tsr/yellow",'r');
	rfd[2] = svc_open("/tsr/blue",'r');
	rfd[3] = svc_open("/tsr/green",'r');
	
	wfd[0] = svc_open("/led/orange",'w');
	wfd[1] = svc_open("/led/yellow",'w');
	wfd[2] = svc_open("/led/blue",'w');
	wfd[3] = svc_open("/led/green",'w');
	while(1)
	{
		char buff[4];
		svc_read(rfd[0],&buff[0]);
		svc_read(rfd[1],&buff[1]);
		svc_read(rfd[2],&buff[2]);
		svc_read(rfd[3],&buff[3]);
		if(buff[0] == '1' && buff[1] == '1' && buff[2] == '1' && buff[3] == '1')
		{
			svc_close(rfd[0]);svc_close(rfd[1]);
			svc_close(rfd[2]);svc_close(rfd[3]);
			svc_close(wfd[0]);svc_close(wfd[1]);
			svc_close(wfd[2]);svc_close(wfd[3]);
			return 0;
		}
		svc_write(wfd[0],buff[0]);
		svc_write(wfd[1],buff[1]);
		svc_write(wfd[2],buff[2]);
		svc_write(wfd[3],buff[3]);
	}
	return 1;
}
static int cmd_pot2ser(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	int rfd = svc_open("/apm",'r');
	if(rfd > 0)
	{
		char buff;int val;
		val = svc_read(rfd,&buff);
		while(val > 0)
		{
			char temp[8];
			sprintf(temp,"%d\r\n",val);
			uprintf(temp);
			val = svc_read(rfd,&buff);
		}
		svc_close(rfd);
		return 0;
	}
	return 1;
}
static int cmd_therm2ser(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	int rfd = svc_open("/ttr",'r');
	if(rfd > 0)
	{
		char buff;int val;
		val = svc_read(rfd,&buff);
		while(!sw1In())
		{
			char temp[8];
			sprintf(temp,"%d\r\n",val);
			uprintf(temp);
			val = svc_read(rfd,&buff);
		}
		svc_close(rfd);
		return 0;
	}
	return 1;
}
static int cmd_pb2led(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	int rfd1 = svc_open("/btn/sw1",'r');
	if(rfd1 < 0)
	{
		return 1;
	}
	
	int rfd2 = svc_open("/btn/sw2",'r');
	if(rfd2 < 0)
	{
		return 1;
	}
	int wfd1 = svc_open("/led/orange",'w');
	if(wfd1 < 0)
	{
		return 1;
	}
	int wfd2 = svc_open("/led/yellow",'w');
	if(wfd2 < 0)
	{
		return 1;
	}
	while(1)
	{
		char buff[2];
		svc_read(rfd1,&buff[0]);
		svc_read(rfd2,&buff[1]);
		if(buff[0] == '1' && buff[1] == '1')
		{
			svc_close(rfd1);
			svc_close(rfd2);
			svc_close(wfd1);
			svc_close(wfd2);
			return 0;
		}
		svc_write(wfd1,buff[0]);
		svc_write(wfd2,buff[1]);
	}
	return 1;
}

static int cmd_list(int argc, char *argv[])
{
	if(argc > 1)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	vfs_list();
	return 0;
}

static int cmd_fopen(int argc, char *argv[])
{
	char tempstr[1024];
	if(argc > 3)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 3 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	int fd = svc_open(argv[1],argv[2][0]);
	if(fd >= 0)
	{
		sprintf(tempstr,"%s opened on file descriptor = %d\r\n",argv[1],fd);uprintf(tempstr);
		return 0;
	}
	sprintf(tempstr,"could not open %s\r\n",argv[1]);uprintf(tempstr);
	return 1;
}

static int cmd_fclose(int argc, char *argv[])
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
	int fd = strtoint(argv[1]);
	svc_close(fd);
	return 0;
}

static int cmd_fgetc(int argc, char *argv[])
{
	char tempstr[1024];
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
	int fd = strtoint(argv[1]);
	if(fd >= 0)
	{
		char buff;
		int bytesread = svc_read(fd,&buff);
		if(bytesread == 1)
		{
			sprintf(tempstr,"%c\r\n",buff);uprintf(tempstr);
			return 0;
		}
	}
	sprintf(tempstr,"could not read file\r\n");uprintf(tempstr);
	return 1;
}

static int cmd_fputc(int argc, char *argv[])
{
	char tempstr[1024];
	if(argc > 3)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 3 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	int fd = strtoint(argv[1]);
	if(fd >= 0)
	{
		char buff = argv[2][0];
		int bytesread = svc_write(fd,buff);
		if(bytesread == 1)
		{
			sprintf(tempstr,"%c written\r\n",buff);uprintf(tempstr);
			return 0;
		}
	}
	sprintf(tempstr,"could not write file\r\n");uprintf(tempstr);
	return 1;
}

static int cmd_create(int argc, char *argv[])
{
	char tempstr[1024];
	if(argc > 3)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 3 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	int fd = svc_create(argv[1],argv[2][0]);
	if(fd >= 0)
	{
		sprintf(tempstr,"%s created and opened on file descriptor = %d\r\n",argv[1],fd);uprintf(tempstr);
		return 0;
	}
	sprintf(tempstr,"could not create %s\r\n",argv[1]);uprintf(tempstr);
	return 1;
}

static int cmd_delete(int argc, char *argv[])
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
	svc_delete(argv[1]);
	return 0;
}

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
	char tempstr[1024];
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
	my_errno = NONE_ERROR;
	void *ptr = svc_malloc(size);
	if(my_errno != NONE_ERROR)
	{
		return 1;
	}
	sprintf(tempstr,"%p\r\n",ptr);uprintf(tempstr);
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
	char tempstr[1024];
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
	my_errno = NONE_ERROR;
	svc_free(ptr);
	if(my_errno != NONE_ERROR)
	{
		return 1;
	}
	sprintf(tempstr,"Memory freed successfully\r\n");uprintf(tempstr);
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

static int cmd_setDate(int argc, char *argv[])
{
	if(argc > 3)
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	if(argc < 3 )
	{
		my_errno = TOO_FEW_ARGUMENTS;
		return 1;
	}
	char dest[1024];
	sprintf(dest,"%s %s",argv[1],argv[2]);
	int result = svc_setDateAndTime(dest);
	if(result == 0)
	{
		struct datetime dtime;
		svc_getDateAndTime(&dtime);
		char tempstr[1024];
		sprintf(tempstr,"Current Date and Time : %s %d, %d %02d:%02d:%02d.%d\r\n",
				getMonthName(dtime.month),dtime.day,dtime.year,dtime.hour,
				dtime.minute,dtime.seconds,dtime.msecs);uprintf(tempstr);
	}
	return result;
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
	struct datetime dtime;
	svc_getDateAndTime(&dtime);
	char tempstr[1024];
	sprintf(tempstr,"%s %d, %d %02d:%02d:%02d.%d\r\n",
			getMonthName(dtime.month),dtime.day,dtime.year,dtime.hour,
			dtime.minute,dtime.seconds,dtime.msecs);uprintf(tempstr);
	return 0;
}

static int cmd_pdbDemo(int argc, char *argv[])
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
	int seconds = strtoint(argv[1]);
	svc_pdbDemo(seconds);
	return 0;
}

static int cmd_forkdemo(int argc, char *argv[])
{
	pid_t pid1,pid2,pid3;
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	struct spawnArgs args;
	args.argc = 1;
	args.argv = NULL;
	args.spawnedPidptr = &pid1;
	args.stackSize = STACK_SIZE;
	args.funcPtr = demoProc1;
	svc_spawn(&args);
	args.funcPtr = demoProc2;
	args.spawnedPidptr = &pid2;
	svc_spawn(&args);
	args.funcPtr = demoProc3;
	args.spawnedPidptr = &pid3;
	svc_spawn(&args);
	svc_wait(pid1);
	return 0;
}

static int cmd_ps(int argc, char *argv[])
{
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	ps_list();
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
	char tempstr[1024];
	int i;
	if(argc > 1)
	{	
		sprintf(tempstr,"%s",argv[1]);uprintf(tempstr);
		for(i = 2;i < argc;++i)
		{
			sprintf(tempstr," %s",argv[i]);uprintf(tempstr);
		}
	}
	sprintf(tempstr,"\r\n");uprintf(tempstr);
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
	char tempstr[1024];
	if(argc > 1)//no arguments allowed, first argument is command name
	{
		my_errno = TOO_MANY_ARGUMENTS;
		return 1;
	}
	sprintf(tempstr,"Command	     Description					Usage\r\n");uprintf(tempstr);
	sprintf(tempstr,"-------	     -----------					-----\r\n");uprintf(tempstr);
	sprintf(tempstr,"date	     Print the date					date\r\n");uprintf(tempstr);
	sprintf(tempstr,"echo	     Echo arguments to the standard output		echo [args]\r\n");uprintf(tempstr);
	sprintf(tempstr,"help	     Print brief description of the shell commands	help\r\n");uprintf(tempstr);
	sprintf(tempstr,"exit	     Quit the shell					exit\r\n");uprintf(tempstr);
	return 0;
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
