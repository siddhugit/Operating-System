/*
 * miscfs.c
 *
 *  Created on: Oct 31, 2015
 *      Author: URM
 */

#include "miscfs.h"
#include "svc.h"
#include "derivative.h"
#include "memory.h"
#include "led.h"
#include "pushbutton.h"
#include "fstring.h"
#include "ts1.h"
#include <stdlib.h>

#define ADC_CHANNEL_POTENTIOMETER   	0x14
#define ADC_CHANNEL_TEMPERATURE_SENSOR  0x1A

struct misc_stream
{
	void* minor;
};

void*apm_open(const char *name,char mode)
{
	struct misc_stream *stream = myMalloc(sizeof(struct misc_stream));
	stream->minor = NULL;
	return stream;
}
int apm_read(void *fp,char* buff)
{
	ADC1_SC1A = ADC_CHANNEL_POTENTIOMETER;
   while(!(ADC1_SC1A & ADC_SC1_COCO_MASK)) {
   }
   *buff = ADC1_RA;
   return ADC1_RA;
}

void* ttr_open(const char *name,char mode)
{
	struct misc_stream *stream = myMalloc(sizeof(struct misc_stream));
	stream->minor = NULL;
	return stream;
}
int ttr_read(void *fp,char* buff)
{
	ADC1_SC1A = ADC_CHANNEL_TEMPERATURE_SENSOR;
   while(!(ADC1_SC1A & ADC_SC1_COCO_MASK)) {
   }
   *buff = ADC1_RA;
   return ADC1_RA;
}

static const char* ONE_TSR = "orange";
static const char* TWO_TSR = "yellow";
static const char* THREE_TSR = "green";
static const char* FOUR_TSR = "blue";

enum TSR_MINOR{TSR_ONE=0,TSR_TWO=1,TSR_THREE=2,TSR_FOUR=4,NTSR};

void* tsr_open(const char *name,char mode)
{
	enum TSR_MINOR minor = NTSR;
	if(strcompare(ONE_TSR,name))
	{
		minor = TSR_ONE;
	}
	else if(strcompare(TWO_TSR,name))
	{
		minor = TSR_TWO;
	}
	else if(strcompare(THREE_TSR,name))
	{
		minor = TSR_THREE;
	}
	else if(strcompare(FOUR_TSR,name))
	{
		minor = TSR_FOUR;
	}
	else
	{
		//my_errno = NOT_A_TSR;
		return NULL;
	}
	struct misc_stream *stream = myMalloc(sizeof(struct misc_stream));
	stream->minor = (void*)minor;
	return stream;
}

int tsr_read(void *fp,char* buff)
{
	//electrode_in(0)
	struct misc_stream* stream = (struct misc_stream*)fp;
	int result = 0;
	switch((enum TSR_MINOR)stream->minor)
	{
		case TSR_ONE:
			result = electrode_in(0);				
			break;
		case TSR_TWO:
			result = electrode_in(1);				
			break;
		case TSR_THREE:
			result = electrode_in(2);				
			break;
		case TSR_FOUR:
			result = electrode_in(3);				
			break;
		default:
			return 0;
	}
	*buff = (result > 0) ? '1' : '0';
	return 1;
	return 1;
}

void misc_close(void *fp)
{
	myFree(fp);
}
void misc_create(const char *name)
{
}
void misc_delete(const char *name)
{
}
int misc_write(void *fp,char c)
{
	return 0;
}
char** misc_list()
{
	char **result = (char**)myMalloc(sizeof(char*));
	result[0] = NULL;
	return result;
}

