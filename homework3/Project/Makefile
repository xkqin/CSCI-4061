
#the C compiler
CC = gcc
CFLAGS =-g 

assignment2: piper.o
	$(CC) -o piper piper.o

Assignment2.o: piper.c
	$(CC) $(CFLAGS) -c piper.c

clean:
	rm -f *.o
	rm -f piper
