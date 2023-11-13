#define _XOPEN_SOURCE 700 // C'est de la magie noire.

#include "utils.h"
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int my_cd(char* cur, char* last, const char* query){
    if (strlen(query) == 2){
        // No query, go home
        strcpy(last, cur);
        strcpy(cur, getenv("HOME"));
        return 0;
    }

    if (strlen(query) == 4 && query[3] == '-'){
        // Go to previous directory
        char* temp = calloc(MAX_STRING_LENGTH, sizeof(char));
        strcpy(temp, cur);
        strcpy(cur, last);
        strcpy(last, temp);
        free(temp);
        return 0;
    }

    struct stat sb;

    char* np = calloc(MAX_STRING_LENGTH, sizeof(char));
    strcat(strcat(strcat(np, cur), "/"), query+  3);

    if (stat(np, &sb) == 0 && (sb.st_mode & S_IFMT) == S_IFDIR){
        // If the path points to an existing directory
        strcpy(last, cur);
        strcat(strcat(cur, "/"), query + 3);

        if (realpath(cur, np) == NULL){exit(0);}
        strcpy(cur, np);

        free(np);
        return 0;
    }
    write(1, query+3, strlen(query)-3);
    write(1, " : No such file or directory\n", strlen(" : No such file or directory\n"));

    free(np);
    return 1;
}