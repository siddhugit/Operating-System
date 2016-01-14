#include <stdlib.h>
#include "uart.h"
#include "uart.h"
#include "svc.h"
#include "memory.h"
#include "intSerialIO.h"

//Button stream class
struct uart_stream
{
	void* minor;
};

void* uart_open(const char *name,char mode)
{
	struct uart_stream *stream = myMalloc(sizeof(struct uart_stream));
	stream->minor = NULL;
	return stream;
}
void uart_close(void *fp)
{
	myFree(fp);
}
void uart_create(const char *name)
{
}
void uart_delete(const char *name)
{
}
int uart_read(void *fp,char* buff)
{
	*buff = getcharFromBuffer();
	return 1;
}
int uart_write(void *fp,char c)
{
	putcharIntoBuffer(c);
	return 1;
}
char** uart_list()
{
	char **result = (char**)myMalloc(sizeof(char*));
	result[0] = NULL;
	return result;
}
