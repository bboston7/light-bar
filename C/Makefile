CC = gcc
CFLAGS = -g -Wall -std=gnu11

all: prog_test solid_white

prog_test: prog_test.c light_bar.o
	$(CC) $(CFLAGS) -o prog_test prog_test.c light_bar.o

solid_white: solid_white.c light_bar.o
	$(CC) $(CFLAGS) -o solid_white solid_white.c light_bar.o

light_bar.o: light_bar.c light_bar.h
	$(CC) $(CFLAGS) -c light_bar.c

clean:
	$(RM) *.o solid_white
