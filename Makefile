target:
	gcc -o server -lpthread server.c

clean:
	rm -f server


.PHONY:clean
