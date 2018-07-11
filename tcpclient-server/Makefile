all: server client mp_server

server:
	gcc -o tcp_server -lpthread tcp_server.c

mp_server:
	gcc -o tcp_server_multiprocess -lpthread tcp_server_multiprocess.c

client:
	gcc -o tcp_client -lpthread tcp_client.c

clean:
	rm -f server tcp_client tcp_server_multiprocess server.log


.PHONY:clean server client 
