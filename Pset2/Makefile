#
# Makefile for shell
#
# Type  make    to compile the shell program
#

####### Compiler and options

CC            = gcc
CFLAGS        = -Wall -g

####### Files

all: shell memory

shell: shell.o dateutil.o commands.o error.o memory.o
	$(CC) $(CFLAGS) -o shell shell.o dateutil.o commands.o error.o memory.o

memory: main.o error.o memory.o
	$(CC) $(CFLAGS) -o memory main.o error.o memory.o

main.o: main.c error.c error.h memory.c memory.h
	$(CC) $(CFLAGS) -c main.c

memory.o: memory.c memory.h error.c error.h
	$(CC) $(CFLAGS) -c memory.c

error.o: error.c error.h 
	$(CC) $(CFLAGS) -c error.c

dateutil.o: dateutil.c dateutil.h
	$(CC) $(CFLAGS) -c dateutil.c

commands.o: commands.c commands.h dateutil.c dateutil.h error.c error.h memory.c memory.h
	$(CC) $(CFLAGS) -c commands.c

shell.o: shell.c commands.c commands.h dateutil.c dateutil.h error.c error.h memory.c memory.h
	$(CC) $(CFLAGS) -c shell.c


clean:
	rm -f *.o shell memory
