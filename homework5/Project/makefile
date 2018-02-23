# Makefile for assignment 5

server: server.o client.o
	gcc -g -o server server.o -lpthread
	gcc -g -o client client.o
	
server.o: quote_server.c
	gcc -g -o server.o -c quote_server.c -lpthread

client: client.o
	gcc -g -o client client.o

client.o: quote_client.c
	gcc -g -o client.o -c quote_client.c

clean:
	rm *.o
	rm server
	rm client
