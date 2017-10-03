These are a few simple examples meant to refresh your knowledge of client and server 
communication over TCP sockets.

Dowload echoapp.tar and extract the files into a Unix directory. 
You should see the following files:

echoserver.c -- C code for a sequential echo server
echoservert.c -- C code for a multi-threaded echo server
echoclient.c -- C code for an echo client
nethelp.c -- File containing helper networking functions
nethelp.h -- Header file for helper networking functions defined in nethelp.c      
Makefile -- Compiles and links together the source files
readme -- this file

1. Build the echoserver, echoservert and echoclient executables by typing in 
make at the shell prompt.

2. Test the client and sequential server on tanner. 
	Pass your designated port number N as an argument to the server:

     	./echoserver N</pre>

	Open a separate window on tanner to test the server with telnet first:

	telnet tanner  N

       	and with the echo client second:

	./echoclient tanner N

	Now anything you type into the client window will be sent over the connection 
	and echoed back to you by the server.

3. Try to understand what each step of the client and server code does.  
Report questions and answers you may generate while experimenting with this code. 

4. The multithreaded version <tt>echoservert.c</tt> spawns a thread for each 
connection from a client, then gets back immediately to listening for
new incoming requests from clients. To test your multithreaded server, open two client 
windows and have the clients talk simultaneously to the server.










