/*
* Jake Mitchell
* Fall 2017 Networks Programming Assignment 2
*/

//deal with directories
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <signal.h>
#include "uthash.h"


#define LINESIZE 1024
#define MAXLINE  2048
#define MAXMESSAGE 819200 //fucking massive mate


struct keyValue {
    char key[LINESIZE];
    char value[LINESIZE];
    UT_hash_handle hh;
};
struct httpRequest {
    char* command;
    char* document;
    char* protocol;
};



void add_key_value(struct keyValue** , char*, char*);
struct keyValue *findKey(struct keyValue**, char*);
char* trimwhitespace(char*);
void parse_config_file(struct keyValue**);
//the thread function
void *connection_handler(void *);
void sig_handler(int);
struct httpRequest parse_http(char**);


int socket_desc;
struct keyValue *hashTable;

int main(int argc, char **argv) {
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        printf("\ncan't catch SIGINT\n");
    }


    int client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    char client_message[MAXLINE];

    hashTable = NULL;

    parse_config_file(&hashTable);

    struct keyValue *listenHash = findKey(&hashTable, "Listen");
    //printf("Port: %s\n", listenHash->value);

    //base code from http://www.binarytides.com/server-client-example-c-sockets-linux/
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    //puts("Socket created");
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(listenHash->value));
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    //puts("bind done");
    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    //puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        //puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned");
        //puts("Waiting for incoming connections...");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    close(socket_desc);
}


void parse_config_file(struct keyValue **hash){
    char * line = NULL;
    FILE *confFile;
    size_t len = 0;
    ssize_t read;

    confFile = fopen("ws.conf", "r");
    if (confFile == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, confFile)) != -1) {
        if(line[0] != '#'){
            //printf("%s", line);
            char* second = strchr(line, ' ');
            size_t lengthOfFirst = second - line;
            char* first = (char*)malloc((lengthOfFirst + 1)*sizeof(char));
            strncpy(first, line, lengthOfFirst);

            first = trimwhitespace(first);
            second = trimwhitespace(second);

            add_key_value(hash, first, second);
        }
    }
    /*struct keyValue *f = findKey(hash, ".jpg");
    printf("%s\n", f->value);*/
    fclose(confFile);
}


void add_key_value(struct keyValue **hash, char* keyIn, char* valueIn) {
    struct keyValue *s;
    HASH_FIND_INT(*hash, &keyIn, s);  //id already in the hash?
    if (s==NULL) {
        s = malloc(sizeof(struct keyValue));
        strcpy(s->key, keyIn);
        strcpy(s->value, valueIn);
        HASH_ADD_STR(*hash, key, s);
    }
}

struct keyValue *findKey(struct keyValue **hash, char* keyId) {
    struct keyValue *s;
    HASH_FIND_STR(*hash, keyId, s);
    return s;
};

//from https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char *trimwhitespace(char *str)
{
  char *end;

  while(isspace((unsigned char)*str)) str++; // Trim leading space

  if(*str == 0)  // All spaces?
    return str;

  end = str + strlen(str) - 1; // Trim trailing space
  while(end > str && isspace((unsigned char)*end)) end--;

  *(end+1) = 0; // Write new null terminator

  return str;
}

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    //printf("received SIGINT\n");
    close(socket_desc);
    exit(0);
  }
}

struct httpRequest parse_http(char** http) {
    char* request;
    struct httpRequest h;


    request = strtok((char * restrict)http, "\n");
    printf("%s\n", request);

    h.command = trimwhitespace(strtok(request, " "));
    h.document = trimwhitespace(strtok(NULL, " "));
    h.protocol = trimwhitespace(strtok(NULL, " "));
    return h;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char* client_message[MAXLINE];
    struct httpRequest request;
    char message[MAXMESSAGE];

    read_size = recv(sock , client_message , MAXLINE , 0);
    if (read_size > 0){
        request = parse_http(client_message);
    }
    if (!strcmp(request.protocol, "HTTP/1.1")){
        strcpy(message, "HTTP/1.1 ");
    } else {
        strcpy(message, "HTTP/1.0 ");
    }

    //printf("|%s|\n", request.command);
    if (!strcmp(request.command, "GET")) {
        struct keyValue *documentRootHash = findKey(&hashTable, "DocumentRoot");
        char documentPath[LINESIZE];
        strcpy(documentPath, documentRootHash->value);

        if (!strcmp(request.document, "/")){
            struct keyValue *baseHash = findKey(&hashTable, "DirectoryIndex");
            strcpy(request.document, "/");
            strcat(request.document, baseHash->value);

        }//default html
        strcat(documentPath, request.document);

        char* contentType = "";
        char* next = strchr(request.document, '.');
        while (next != NULL) {
            contentType = next;
            char* chopped = contentType+1;
            next = strchr(chopped, '.');
        }

        FILE *fp;
        if ((fp = fopen(documentPath, "rb"))){
            char * line = NULL;
            size_t len = 0;

            strcat(message, "200 Document Follows\nContent-type: <");
            struct keyValue *typeHash = findKey(&hashTable, contentType);
            strcat(message, typeHash->value);
            strcat(message, ">\nContent-Length: <");

            fseek(fp, 0L, SEEK_END);
            char numbytes[32];
            int size = ftell(fp);
            sprintf(numbytes, "%d", size);
            printf("file length:%d\n", size);
            strcat(message, numbytes);
            rewind(fp);
            strcat(message, ">\n\n");

            //THIS WORKS FOR TEXT AND MAIN.  DON"T FUCK WITH IT
            /*char* string[MAXMESSAGE];
            fread(string, 1, sizeof(string), fp);
            strcat(message, string);*/

            printf("message:%d\n", strlen(message));
            printf("goal message length:%d\n", size+strlen(message));

            /*char buffer[LINESIZE];
            memset(buffer, 0, sizeof(buffer));
            int readBytes;
            int count = 0;
            while ((readBytes = fread(&buffer,sizeof(char),sizeof(buffer), fp)) > 0) {
                //printf("%s\n\n", buffer);
                count += readBytes;
                printf("readBytes:%d\n", sizeof(readBytes));
                strcat(message, buffer);
                memset(buffer, 0, sizeof(buffer));
            }
            printf("%d\n", count);*/

            /*unsigned char* buffer;
            buffer = malloc(size);
            memset(*buffer, 0, sizeof(*buffer));
            fread(buffer, sizeof(unsigned char), size, fp);
            printf("size of buffer:%s\n", strlen(*buffer));
            printf("size of buffer:%s\n", sizeof(*buffer));*/

            /*char* buffer = (char *)malloc(size+1);
            fread(buffer, size, 1, fp);
            printf("size of buffer:%d\n", sizeof(buffer));
            //printf("size of buffer:%s\n", strlen(buffer));*/

            send(sock , message , strlen(message), 0);
            char buffer[LINESIZE];
            size_t read;
            while ((read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                send(sock, buffer, (int)read, 0);
            }

            fclose(fp);

        } else { //file was not able to be opened
            printf("unable to open.\n");
            strcat(message, "404 Not Found\n\n<html><body>404 Not Found ");
            strcat(message, request.document);
            strcat(message, "</body></html>");
            send(sock , message , strlen(message), 0);
        }

        //printf("%s\n", message);
        //printf("%d\n", strlen(message));


    } else {
        strcat(message, "501 Not Implemented\n\n<html><body>501 Not Implemented ");
        strcat(message, request.command);
        strcat(message, "</body></html>");
        send(sock , message , strlen(message), 0);
    }

    //Free the socket pointer
    free(socket_desc);
    close(sock);

    printf("closed\n");
    return 0;
}
