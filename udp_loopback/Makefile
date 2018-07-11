all: server client

server:
	gcc -o server -lpthread server.c

client:
	gcc -o client -lpthread -g client.c

clean:
	rm -f client server


.PHONY:server clean client 
