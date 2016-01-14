#include <stdlib.h>
#include "uart.h"
#include "uart.h"
#include "svc.h"
#include "memory.h"

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
	*buff = uartGetchar(UART2_BASE_PTR);
	return 1;
}
int uart_write(void *fp,char c)
{
	uartPutchar(UART2_BASE_PTR, c);
	return 1;
}
char** uart_list()
{
	return NULL;
}
