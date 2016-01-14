#ifndef _COMMAND_H__
#define _COMMAND_H__

typedef int (*commandfunc_t)(int argc, char *argv[]);

//runs shell command
int run_command(int argc, char *argv[]);
commandfunc_t find_command(const char* cmdName);
//int fprintf(FILE *stream, const char *format, ...);

#endif //_COMMAND_H__
