
#the C compiler
CC = gcc
CFLAGS =

assignment2: Assignment2.o
	$(CC) -o assignment2 Assignment2.o

Assignment2.o: Assignment2.c
	$(CC) $(CFLAGS) -c Assignment2.c

clean:
	rm -f *.o
	rm -f assignment2
