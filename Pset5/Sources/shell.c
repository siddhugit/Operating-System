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
#include <stdint.h>
#include "commands.h"
#include "error.h"
#include "memory.h"
#include "vfs.h"
#include "led.h"
#include "pushbutton.h"
#include "mcg.h"
#include "sdram.h"
#include "uart.h"
#include "svc.h"
#include "lcdc.h"
#include "ustream.h"
#include "lcdcConsole.h"
#include "ustream.h"
#include "ts1.h"
#include "priv.h"
#include "flexTimer.h"
#include "intSerialIO.h"
#include "dateutil.h"

extern struct console console;

//static const uint32_t ONE_MEGA_BYTE  = (1<<20);
static const uint32_t STATIC_RAM_MEMORY  = (1<<15);

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
	char tempstr[1024];
	int i;//source string iterator
	char* newstr = NULL;//destination string
	//allocate memory for arguments, caller is responsible to free memory
	newstr = (char*)(myMalloc((size + 1)*sizeof(char)));
	if(newstr == NULL)//if malloc fails
	{
		sprintf(tempstr,"myMalloc failed: %s\r\n",strerror(errno));uprintf(tempstr);
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
		myFree(argv[i]);//free each argument
		argv[i] = NULL;//just in case, free NULL would be no-op
	}
	myFree(argv);//free array
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
	char tempstr[1024];
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
	argv = (char**)(myMalloc((*numargs)*sizeof(char*)));
	if(argv == NULL)
	{
		sprintf(tempstr,"myMalloc failed: %s\r\n",strerror(errno));uprintf(tempstr);
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
	char tempstr[1024];
	char c;		//input char
	char line[LINE_MAX + 1]; //command line temporary buffer
	int pos = 0;
	//sprintf(UBUFFER,"%s", prompt);
	sprintf(tempstr,"%s", prompt);
	uprintf(tempstr);	//prompt user	
	while( ( c = ugetc()) != EOF && pos < LINE_MAX) 
	{
		//end of command
		if ( c == '\r')
		{
			//uprintf("\r\n");
			break;
		}
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
	char tempstr[1024];
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
				sprintf(tempstr,"%s\r\n",my_strerr(my_errno));uprintf(tempstr);
			}
			//destroy arguments
			arguments_destroy(argc,argv);
		}
	}
	return 0;
}

#define ADC_CFG1_MODE_8_9_BIT       0x0
#define ADC_CFG1_MODE_12_13_BIT     0x1
#define ADC_CFG1_MODE_10_11_BIT     0x2
#define ADC_CFG1_MODE_16_BIT        0x3
#define ADC_SC3_AVGS_32_SAMPLES     0x3

void adc_init(void) {
   SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
   ADC1_CFG1 = ADC_CFG1_MODE(ADC_CFG1_MODE_12_13_BIT);
   ADC1_SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(ADC_SC3_AVGS_32_SAMPLES);
}

static void bootstrap()
{
	const int moduleClock = 12000000;
	const int KHzInHz = 1000;
	const int baud = 115200;
	mcgInit();
	sdramInit();
	svcInit_SetSVCPriority(15);
	uartInit(UART2_BASE_PTR, moduleClock/KHzInHz, baud);
	lcdcInit();
	lcdcConsoleInit(&console);
	adc_init();
	vfs_init();
	ledInitAll();
	pushbuttonInitAll();
	TSI_Init();
	TSI_Calibrate();
	init_memory();
	uinit();
	intSerialIOInit();
	flexTimerInit();
}

int main(int argc,char *argv[])
/*
 * main function of the shell program, calls process_input(). As process_input()
 *	does all the error reporting and does not propagate errors out of it
 *	and shell program does not have to return any failure exit status
 *	to its parent shell, main() simply retuns 0    
*/
{
	//Bootstrap
	bootstrap();
	//flexTimer0Start();

	//process input
	process_input();
	return 0;
}
