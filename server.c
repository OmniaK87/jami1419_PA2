/*
* Jake Mitchell
* Fall 2017 Networks Programming Assignment 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"

#define LINESIZE 1024

struct keyValue {
    char key[LINESIZE];
    char value[LINESIZE];
    UT_hash_handle hh;
};

void add_key_value(char* key, char* value);



int main(int argc, char **argv) {

    FILE *confFile;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;


    confFile = fopen("ws.conf", "r");
    if (confFile == NULL)
        exit(EXIT_FAILURE);

    struct my_struct *hashTable = NULL;
    printf("adding first item to hash.\n");
    add_key_value(*hashTable, "test", "testing");
    printf("done adding first item to hash.\n");


    while ((read = getline(&line, &len, confFile)) != -1) {
        if(line[0] != '#'){
            printf("%s", line);
        }
    }

    fclose(confFile);

}

void add_key_value(struct keyValue **hash, char* keyIn, char* valueIn) {
    struct keyValue *s;
    HASH_FIND_INT(hashTable, &keyIn, s);  /* id already in the hash? */
    if (s==NULL) {
        s = malloc(sizeof(struct keyValue));
        strcpy(s->key, keyIn);
        strcpy(s->value, valueIn);
        HASH_ADD_STR(hash, key, s);
    }
}

