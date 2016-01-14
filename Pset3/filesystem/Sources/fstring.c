#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "cwfix.h"
#include "memory.h"


int strtoint(const char *str)
{
	int num=0;
	while(*str)
	{
		if((*str) < '0' || (*str) > '9')
		{
			return -1;
		} 
		num=((*str)-'0')+num*10;
		++str;   
	}
	return num;
}

bool strcompare(const char *str1,const char *str2)
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

int str_len(const char *str)
{
	int size = 0;
	while(*str)
	{
		++size;++str;
	}
	return size;
}
bool startsWith(const char *str1,const char *str2)
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
	int i;
	int len = str_len(str2);
	for(i = 0;i < len; ++i)
	{	
		if(*str1)
		{
			if(*str1 != *str2)//any character mismatch
				return 0;
			++str1;++str2;
		}
		else
		{	
			return 0;
		}
	}	
	return 1;//strings are equal
}
char *new_str(const char *source)
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
	int size = str_len(source);
	//allocate memory for arguments, caller is responsible to free memory
	newstr = (char*)(myMalloc((size + 1)*sizeof(char)));
	if(newstr == NULL)//if malloc fails
	{
		printf("myMalloc failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	for(i = 0;i < size;++i)
	{
		newstr[i] = source[i];//copy from source
	}
	newstr[size] = '\0';//terminate string
	return newstr;
}

static bool ischaralpha(char c)
{
        return((c >='a' && c <='z') || (c >='A' && c <='Z'));
}
static bool isnumeric(char c)
{
        return(c >='0' && c <='9');
}

bool validateFileName(const char* fname)
{
	if(fname == NULL)
			return 0;	
		int i;
		int len = str_len(fname);
		if(len == 0)
		{
			return false;
		}
		if(!ischaralpha(fname[0]))
		{
			return false;
		}
		for(i = 1;i < len; ++i)
		{
			if(!ischaralpha(fname[i]) && !isnumeric(fname[i]))
			{
				return false;
			}
		}
		return true;
}

