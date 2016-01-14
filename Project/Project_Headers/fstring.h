#include "cwfix.h"

#ifndef _FSTRING_H__
#define _FSTRING_H__


bool strcompare(const char *str1,const char *str2);
char *new_str(const char *source);
int str_len(const char *str);
int strtoint(const char *str);
bool startsWith(const char *str1,const char *str2);
bool validateFileName(const char* fname);
char *strtokenizer(char* source,const char *delim,int *found);

#endif //_FSTRING_H__

