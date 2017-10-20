/*
* Jake Mitchell
* Fall 2017 Networks Programming Assignment 2
*/


//Currently have config file in, with values having a leading space.

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



void add_key_value(struct keyValue** , char*, char*);
struct keyValue *findKey(struct keyValue**, char*);



int main(int argc, char **argv) {

    FILE *confFile;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    struct keyValue *hashTable = NULL;

    confFile = fopen("ws.conf", "r");
    if (confFile == NULL)
        exit(EXIT_FAILURE);


    /*printf("adding first item to hash.\n");
    add_key_value(&hashTable, "test", "testing");
    printf("done adding first item to hash.\n");
    struct keyValue *f = findKey(&hashTable, "test");
    printf("%s\n", f->key);*/


    while ((read = getline(&line, &len, confFile)) != -1) {
        if(line[0] != '#'){
            //printf("%s", line);
            char* second = strchr(line, ' ');
            size_t lengthOfFirst = second - line;
            char* first = (char*)malloc((lengthOfFirst + 1)*sizeof(char));
            strncpy(first, line, lengthOfFirst);

            add_key_value(&hashTable, first, second);
        }
    }

    struct keyValue *f = findKey(&hashTable, ".jpg");
    printf("%s\n", f->value);

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

