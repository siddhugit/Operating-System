/*
 * dateutil.c
 *
 *  Created on: Nov 15, 2015
 *      Author: URM
 */

/**
 **     This is utility file which implements conversion of epoch time to
 **	readable date and time. It has one public interface which
 **	takes a struct representing epoch time. It encapsulates implementation
 **	of epoch time to readable date and time conversion algorithm described 
 **	in the Outline document
 **/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "dateutil.h"
#include "error.h"

//shell program errno declaration
extern enum Shell_ErrNo my_errno;

typedef long long int time_t;

struct timeval
{
	time_t tv_sec;
	time_t tv_usec;
};

static time_t CURRENT_TIMEVAL = 0;

//number of seconds in a non-leap year
static const time_t REGULAR_YEAR_SECONDS = 31536000;//365 days
//number of seconds in a leap year
static const time_t LEAP_YEAR_SECONDS = 31622400;//366 days
//number of seconds in month of February in a leap year
static const time_t LEAP_FEBRUARY_SECONDS = 2505600;//29 days
//number of seconds in months of a non-leap year
static const time_t MONTH_SECONDS[] = {
				2678400,//31 days
				2419200,//28 days
				2678400,	
				2592000,//30 days
				2678400,
				2592000,
				2678400,
				2678400,
				2592000,
				2678400,
				2592000,
				2678400};
//int to month name
static const char* MONTH_NAMES[] = {
				"January",
				"February",
				"March",
				"April",
				"May",
				"June",
				"July",
				"August",
				"September",
				"October",
				"November",
				"December",
			    };	
//epoch time start year
static const int START_YEAR = 1900;

#define ONE_MILI_SECONDS 1000

static int isLeapYear(int year)
/*
*   checks if year is leap or not
*   Parameters:
*       o int year- year 
*   Return : 1 if a leap year, otherwise 0
*/
{
	if((year % 4) != 0){//not a leap year if not multiple of 4
		return 0;
	}
	if((year % 100) == 0 && (year % 400) != 0){
		return 0;//not a leap year if multiple of 100 but not 400
	}
	return 1;//leap year otherwise
}

static time_t getSecondsInYear(int isLeap)
/*
*   returns number of seconds of an year
*   Parameters:
*       o int isLeap - boolean value to which says whether year is leap or not 
*   Return : number of seconds of an year
*/
{
	return ((isLeap == 1) ? LEAP_YEAR_SECONDS : REGULAR_YEAR_SECONDS);
}

static time_t getYear(struct timeval now,int *currentYear)
/*
*   Implements epoch time to year conversion as per the algorithm defined 
*		in the Outline document
*   Parameters:
*       o struct timeval now - epoch time  
*	o int *currentYear - [out] function fills in current year value 
*		in currentYear  
*   Return : number of seconds left since start of current year
*/
{
	*currentYear = START_YEAR;
        int isALeapYear = 0;
        time_t secondsInCurrentYear,secondsLeft;
        secondsLeft = now.tv_sec;
	//check if current year is a leap year
        isALeapYear = isLeapYear(*currentYear);
	//find seconds in current year
        secondsInCurrentYear = getSecondsInYear(isALeapYear);
	/*keep subtracting until seconds left is less that seconds in 
	iterator year*/
        while(secondsLeft >= secondsInCurrentYear){
                ++(*currentYear);
		secondsLeft -= secondsInCurrentYear;//seconds left
                isALeapYear = isLeapYear(*currentYear);
                secondsInCurrentYear = getSecondsInYear(isALeapYear);
        }
	return secondsLeft;//return seconds left since start of current year
}

static time_t getMonthSeconds(int month,int isLeapYear)
/*
*   returns number of seconds in a month of a given year
*   Parameters:
*	o int month - month number
*       o int isLeapYear - boolean value to which says whether year is 
*		leap or not 
*   Return : number of seconds in a month of a given year
*/
{
	//if leap year and month is February
	if(isLeapYear == 1 && month == 1){
		return LEAP_FEBRUARY_SECONDS;
	}
	return MONTH_SECONDS[month];
}

static int getMonth(time_t secondsLeft,int currentYear,int *currentMonth)
/*
*   Implements conversion of number of seconds left since start of current year   
*		to month as per algorithm defined in the Outline document
*   Parameters:
*       o time_t secondsLeft - number of seconds left since start of current
*		year
*	o int currentYear - current year   
*	o int *currentMonth - [out] function fills in current month value 
*		in currentMonth
*   Return : number of seconds left since start of current year and month
*/
{
	int isALeapYear = 0;	
	*currentMonth = 0;
	//find seconds in current month
	time_t secondsInCurrentMonth = getMonthSeconds(*currentMonth,isALeapYear);
	isALeapYear = isLeapYear(currentYear);
	/*keep subtracting until seconds left is less that seconds in 
	iterator month*/
        while(secondsLeft >= secondsInCurrentMonth){
                ++(*currentMonth);
                secondsLeft -= secondsInCurrentMonth;
                secondsInCurrentMonth = getMonthSeconds(*currentMonth,isALeapYear); 
        }
	return secondsLeft;//return seconds left since start of current year and month
}

static int getDate(struct timeval now,struct datetime* dtime)
/*
*   Private helper function to print current date and time
*   Parameters:
*       o struct timeval now - epoch time
*   Return : 0 always ( executes only non-throwing instructions)
*/
{
	int year,month,hour,day,minute,seconds;
	time_t secondsLeft = getYear(now,&year);//get year
        secondsLeft = getMonth(secondsLeft,year,&month);//get month
	//trivial arithmetic for day/hour/minutes/seconds calculation
        seconds = secondsLeft % 60;
        secondsLeft /= 60;
        minute = secondsLeft % 60;
        secondsLeft /= 60;
        hour = secondsLeft % 24;
        secondsLeft /= 24;
        day = secondsLeft + 1;
	//print formatted date and time
	//fprintf(stdout,"%s %d, %d %02d:%02d:%02d.%ld\n",MONTH_NAMES[month],day,year,hour,minute,seconds,now.tv_usec);
    dtime->year = year;dtime->month = month;
    dtime->day = day;dtime->hour = hour;
    dtime->minute = minute;dtime->seconds = seconds;
    dtime->msecs = now.tv_usec;
	return 0;
}

static int gettimeofday(struct timeval *now)
{
	now->tv_sec = CURRENT_TIMEVAL / ONE_MILI_SECONDS;
	now->tv_usec = CURRENT_TIMEVAL % ONE_MILI_SECONDS;
	return 0;
}

static int validateDateTimeofDay(struct datetime now)
{
	if(now.month > 11 || now.month < 0)
		return 1;
	if(now.year > 3000 || now.year < 1900)
		return 1;
	if(now.day > 31 || now.day < 0)
		return 1;
	if(now.hour > 23 || now.hour < 0)
		return 1;
	if(now.minute > 59 || now.minute < 0)
		return 1;
	if(now.seconds > 59 || now.seconds < 0)
		return 1;
	if(now.msecs > 999 || now.msecs < 0)
		return 1;
	return 0;
}

static void setDateTimeofDay(struct datetime now)
{		
	time_t elapsedSecs = 0; 
	int isLeap;
	int currentYear = START_YEAR,currMonth = 0;
	while(currentYear < now.year)
	{
		elapsedSecs += getSecondsInYear(isLeapYear(currentYear));
		++currentYear;
	}
	isLeap = isLeapYear(currentYear);
	while(currMonth < now.month)
	{
		elapsedSecs +=  getMonthSeconds(currMonth,isLeap);
		++currMonth;
	}
	elapsedSecs +=  (now.day - 1) * 24 * 3600; //day seconds
	elapsedSecs +=  (now.hour) * 3600; //hour seconds
	elapsedSecs +=  (now.minute) * 60; //minute seconds
	elapsedSecs +=  now.seconds; //seconds
	CURRENT_TIMEVAL = elapsedSecs*ONE_MILI_SECONDS + now.msecs;
}

int getDateTime(struct datetime* dtime)
/*
*   The only public interface which prints current date and time on standard
*	output by converting from epoch time using internal functions.
*   Return : 0 if successful, otherwise 1 if error occurs in gettimeofday()
*   Reports gettimeofday() system call error if happens
*/

{
	struct timeval now;
	gettimeofday(&now);
	return getDate(now,dtime);
}

int setDateTime(const char* dateTimeStr)
{
	int month,day,year,hour,minute,seconds,msecs;
	struct datetime now;
	sscanf( dateTimeStr, "%02d/%02d/%d %02d:%02d:%02d.%d", &month,&day,&year,&hour,&minute,&seconds,&msecs );
	now.month  = month - 1;
	now.year  = year;
	now.day  = day;
	now.hour  = hour;
	now.minute  = minute;
	now.seconds  = seconds;
	now.msecs  = msecs;
	if(validateDateTimeofDay(now) != 0)
	{
		my_errno = INVALID_DATE_FORMAT;
		return 1;
	}
	setDateTimeofDay(now);
	return 0;
}

const char* getMonthName(int month)
{
	return MONTH_NAMES[month];
}

void updateTime()
{
	++CURRENT_TIMEVAL;//does minimal work in ISR
}
