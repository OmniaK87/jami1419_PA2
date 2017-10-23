CC = gcc

server:
	$(CC) -pthread -g -o server -lm server.c

clean:
	rm -f server
