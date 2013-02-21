all: server client

grid.o: grid.c grid.h
	gcc -g -c grid.c

client.o: client.c client.h
	gcc -g -c client.c

server.o: server.c server.h 
	gcc -g -c server.c

server: server.o grid.o
	gcc -g server.o grid.o -o server

client: client.o
	gcc -g -o client client.o

clean:
	rm *.o
