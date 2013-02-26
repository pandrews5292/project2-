all: server client

grid.o: grid.c grid.h
	gcc -g -c grid.c

queue.o: queue.c queue.h
	gcc -g -c queue.c

client.o: client.c client.h
	gcc -g -c client.c

server.o: server.c server.h 
	gcc -g -c server.c

server: server.o grid.o queue.o
	gcc -g server.o grid.o queue.o -o server -pthread

client: client.o
	gcc -g client.o -o client

cleanexe:
	
	rm server
	rm client
cleano: 
	rm *.o
cleanemacs: 
	rm *~
