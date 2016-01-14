#
# Makefile for shell
#
# Type  make    to compile the shell program
#

####### Compiler and options

CC            = cc
CFLAGS        = -Wall

####### Files


shell: main.o dateutil.o commands.o error.o
	$(CC) $(CFLAGS) -o shell main.o dateutil.o commands.o error.o 

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

dateutil.o: dateutil.c dateutil.h
	$(CC) $(CFLAGS) -c dateutil.c

commands.o: commands.c commands.h dateutil.c dateutil.h error.c error.h
	$(CC) $(CFLAGS) -c commands.c

main.o: main.c commands.c commands.h dateutil.c dateutil.h error.c error.h
	$(CC) $(CFLAGS) -c main.c


clean:
	rm -f *.o shell
