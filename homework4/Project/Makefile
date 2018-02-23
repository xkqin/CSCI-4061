
#the C compiler
CC = gcc
CFLAGS =-g

ass4: ass4.o
	$(CC) -o ass4 ass4.o -lpthread

ass4.o: ass4.c
	$(CC) $(CFLAGS) -c ass4.c 

clean:
	rm -f *.o
	rm -f ass4
