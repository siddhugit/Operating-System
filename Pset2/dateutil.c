/**
 **     This is utility file which implements conversion of epoch time to
 **	readable date and time. It has one public interface which
 **	takes a struct representing epoch time. It encapsulates implementation
 **	of epoch time to readable date and time conversion algorithm described 
 **	in the Outline document
 **/

#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

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
static const int START_YEAR = 1970;

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

static int getDate(struct timeval now)
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
	fprintf(stdout,"%s %d, %d %02d:%02d:%02d.%ld\n",MONTH_NAMES[month],day,year,hour,minute,seconds,now.tv_usec);
	return 0;
}
int print_date()
/*
*   The only public interface which prints current date and time on standard
*	output by converting from epoch time using internal functions.
*   Return : 0 if successful, otherwise 1 if error occurs in gettimeofday()
*   Reports gettimeofday() system call error if happens
*/

{
	struct timeval now;
	if(gettimeofday(&now,NULL) != 0){
		fprintf(stderr,"Error calling gettimeofday(): %s\n",strerror(errno));
		return 1;
	}
	return getDate(now);
}

