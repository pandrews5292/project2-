all: client server

client: client.o client.h
	gcc -g -o client client.o

server: server.o
	gcc -g -o server server.o

server.o: server.c server.h
	gcc -g -c server.c

client.o: client.c client.h
	gcc -g -c client.c

clean:
	rm *.o