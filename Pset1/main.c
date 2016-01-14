/**
 **     This file has main() function of the shell program. Gets input from
 **	standard input, parses it and dynamically allocates memory for 
 **	arguments, calls command handler module to invoke command and passes it
 **	the arguments. Main function never returns, the only way to quit from
 **	the shell program is by running exit command.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "commands.h"
#include "error.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

//shell program prompt symbol
#define	CMD_PROMPT	"$ "
//returns true if x is a whitespace character
#define	is_delim(x) ((x)==' '||(x)=='\t')

//maximum length of command line input
static const int LINE_MAX = 256;

//returns shell program prompt symbol
static const char* getPrompt()
{
    return  CMD_PROMPT;
}

static char *new_str(const char *source,int size)
/*
*   Private helper function to create a copy of source string on heap 
*   Parameters:
*       o const char *source - null terminated source string
*       o int size - size of source string  
*   Return : dynamically allocated copy of source string
*   Caller of this function is responsible for freeing the dynamically 
*   allocated memory for string
*/
{
	int i;//source string iterator
	char* newstr = NULL;//destination string
	//allocate memory for arguments, caller is responsible to free memory
	newstr = (char*)(malloc((size + 1)*sizeof(char)));
	if(newstr == NULL)//if malloc fails
	{
		fprintf(stderr,"Malloc failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	for(i = 0;i < size;++i)
	{
		newstr[i] = source[i];//copy from source
	}
	newstr[size] = '\0';//terminate string
	return newstr;
}

static void arguments_destroy(int argc,char **argv)
/*
*   Private helper function to free memory allocated dynamically for
*   arguments
*   Parameters:
*       o int argc - number of arguments
*       o char **argv - array of argument strings   
*   Return : void
*/
{
	int i;
	for(i = 0;i < argc;++i)
	{
		free(argv[i]);//free each argument
		argv[i] = NULL;//just in case, free NULL would be no-op
	}
	free(argv);//free array
}

static char **arguments_create(const char *line,int *numargs)
/*
*   Parses command line input where arguments are whitespace separated
*   and allocates memory for arguments dynamically. 
*   Parameters:
*       o const char *line - command line input
*       o int *numargs - [out] function fills number of arguments in numargs  
*   Return : dynamically allocated array of arguments
*   Caller of this function is responsible for freeing the dynamically 
*   allocated array of arguments
*/
{
	const char *cp = line;	//pos in string
	const char *start; //start of next argument in line
	char *temp[LINE_MAX];
	char **argv = NULL;//array of string of arguments
	int len,i;//length of next argument 
	*numargs = 0;//initialize out parameter
	while( *cp != '\0' )
	{
		while ( is_delim(*cp) )	//skip leading spaces	
			cp++;
		if ( *cp == '\0' )	//end of string
			break;		// yes, get out	

		// mark start, then find end of word
		start = cp;
		len   = 1;
		while (*++cp != '\0' && !(is_delim(*cp)) )
			len++;//end of word
		//allocate memory for each argument
		temp[(*numargs)++] = new_str(start,len);
	}
	//allocate memory for array of arguments
	argv = (char**)(malloc((*numargs)*sizeof(char*)));
	if(argv == NULL)
	{
		fprintf(stderr,"Malloc failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	for(i = 0;i < (*numargs);++i)
	{
		argv[i] = temp[i];//copy pointer to arguents in array
	}
	return argv;
}


static char **next_cmd(const char *prompt,int *numargs)
/*
 * purpose: read next command line from stdin and parses it by calling the  
*            helper function arguments_create
 * Parameters:
*       o const char *prompt - prompt for shell program
*       o int *numargs - [out] function fills number of arguments in numargs  
*   Return : dynamically allocated array of arguments by calling the helper 
*            function arguments_create
*   Caller of this function is responsible for freeing the dynamically 
*   allocated array of arguments  
 */
{
	char c;		//input char
	char line[LINE_MAX + 1]; //command line temporary buffer
	int pos = 0;
	fprintf(stdout,"%s", prompt);	//prompt user	
	while( ( c = getc(stdin)) != EOF && pos < LINE_MAX) 
	{
		//end of command
		if ( c == '\n' )
			break;
		// add input character to buffer
		line[pos++] = c;
	}
	if ( pos == 0  )		//no input
		return NULL;			//terminate string
	line[pos] = '\0';			// terminate string
	return arguments_create(line,numargs);//create arguments and return
}

static int process_input()
/*
 * purpose: runs the main loop of the shell program and in each loop iteration
	    takes input from user and execute it by calling command module
*   Return : 0 as this funtion does not prpagate errors out of it. Gets the
*		the status of last command ran and reports error if any.
*   This funtion gets the input, create arguments, run command and finally
*	free memory allocated for arguments.Hence this function guarantees that 
	it will release memory it has acquired.   
*/
{
	char **argv;//command arguments
	const char* prompt = getPrompt();//shell program prompt
	int argc;//number of command arguments
	while(1)
	{
		//get next command and create aruments
		if ( (argv = next_cmd(prompt,&argc)) != NULL )
		{
			//run command
			if(run_command(argc,argv) != 0)
			{
				fprintf(stderr,"%s\n",my_strerr(my_errno));
			}
			//destroy arguments
			arguments_destroy(argc,argv);
		}
	}
	return 0;
}

int main(int argc,char *argv[])
/*
 * main function of the shell program, calls process_input(). As process_input()
 *	does all the error reporting and does not propagate errors out of it
 *	and shell program does not have to return any failure exit status
 *	to its parent shell, main() simply retuns 0    
*/
{
	//process input
	process_input();
	return 0;
}
