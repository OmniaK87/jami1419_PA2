CC = gcc

server:
	$(CC) server.c -o server -lm

clean:
	rm -f server
