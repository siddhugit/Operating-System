/**
 **     This module implements a concrete file system interface for LEDs 
 **/

#include <stdlib.h>
#include "derivative.h"
#include "led.h"
#include "ledfs.h"
#include "memory.h"
#include "fstring.h"
#include "error.h"



//shell program errno declaration
extern enum Shell_ErrNo my_errno;

enum LED_MINOR{YELLOW=0,BLUE=1,GREEN=2,ORANGE=3,NLED};

static const char* ORANGE_LED = "orange";
static const char* YELLOW_LED = "yellow";
static const char* GREEN_LED = "green";
static const char* BLUE_LED = "blue";

static const char* LED_PATHS[] = {
		"/led/yellow",
		"/led/blue",
		"/led/green",
		"/led/orange"		
};

#define NUM_LEDS (sizeof(LED_PATHS) / (sizeof(LED_PATHS[0])))

//Button stream class
struct led_stream
{
	enum LED_MINOR minor;
};
/*
*   Implements open interface
*   Parameters:
*       o const char *name - led name
*       o char mode - 'rR','wW' or 'aA' 
*   Return : stream object if successful, otherwise NULL
*   sets my_errno to NOT_A_LED if led name is invalid.
*/
void* led_open(const char *name,char mode)
{
	enum LED_MINOR minor = NLED;
	if(strcompare(ORANGE_LED,name))
	{
		minor = ORANGE;
	}
	else if(strcompare(YELLOW_LED,name))
	{
		minor = YELLOW;
	}
	else if(strcompare(GREEN_LED,name))
	{
		minor = GREEN;
	}
	else if(strcompare(BLUE_LED,name))
	{
		minor = BLUE;
	}
	else
	{
		my_errno = NOT_A_LED;
		return NULL;
	}
	struct led_stream *stream = myMalloc(sizeof(struct led_stream));
	stream->minor = minor;
	return stream;
}
/*
*   Implements close interface
*   Parameters:
*       o void *fp - stream to close.
*/
void led_close(void *fp)
{
	myFree(fp);
}
/*
*   Implements read interface.
*   Parameters:
*       o void *fp - stream to read.
*       o char* buff - character read into buff.
*       Return : Number of bytes read
*/
int led_read(void *fp,char* buff)
{
	struct led_stream* stream = (struct led_stream*)fp;
	int result = 0;
	switch(stream->minor)
	{
		case ORANGE:
			result = PTA_BASE_PTR->PDOR & (1 << LED_ORANGE_PORTA_BIT);				
			break;
		case YELLOW:
			result = PTA_BASE_PTR->PDOR & (1 << LED_YELLOW_PORTA_BIT);				
			break;
		case GREEN:
			result = PTA_BASE_PTR->PDOR & (1 << LED_GREEN_PORTA_BIT);				
			break;
		case BLUE:
			result = PTA_BASE_PTR->PDOR & (1 << LED_BLUE_PORTA_BIT);				
			break;
		default:
			return 0;
	}
	*buff = (result > 0) ? '0' : '1';
	return 1;
}
/*
*   Implements write interface.
*   Parameters:
*       o void *fp - stream to read.
*       o char c - Expects 1 to turn on led and 0 to turn off.
*       Return : Number of bytes written
*/
int led_write(void *fp,char c)
{
	struct led_stream* stream = (struct led_stream*)fp;
	int val = c - '0';
	if(val)
	{
		switch(stream->minor)
		{
			case ORANGE:
				ledOrangeOn();				
				break;
			case YELLOW:
				ledYellowOn();				
				break;
			case GREEN:
				ledGreenOn();				
				break;
			case BLUE:
				ledBlueOn();				
				break;
			default:
				return 0;
		}
	}
	else
	{
		switch(stream->minor)
		{
			case ORANGE:
				ledOrangeOff();				
				break;
			case YELLOW:
				ledYellowOff();				
				break;
			case GREEN:
				ledGreenOff();				
				break;
			case BLUE:
				ledBlueOff();				
				break;
			default:
				return 0;
		}
	}
	return 1;
}
/*
*   Implements create interface. Does nothing because LEDs already created in bootstrap
*/
void led_create(const char *name)
{
}
/*
*   Implements create interface. Does nothing because LEDs exists throughout application
*/
void led_delete(const char *name)
{
}
/*
*   Implements list interface. 
*   Return : Null terminated array of led names.
*/
char** led_list()
{
	char **result;int i = 0;
	int sz = NUM_LEDS;
	result = (char**)myMalloc((sz + 1)*sizeof(char*));
	for(i = 0;i < sz;++i)
	{
		result[i] = LED_PATHS[i];
	}
	result[sz] = NULL;
	return result;
}
