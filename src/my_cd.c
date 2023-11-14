#define _XOPEN_SOURCE 700 // C'est de la magie noire.

#include "utils.h"
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int my_cd(char* cur, char* last, const char* query){
    // Booléen indiquant si le chemin cherché est absolu ou relatif
    int abs = (int)(strlen(query) > 3 && query[3] == '/');

    if (strlen(query) == 2 || (strlen(query) == 4 && query[3] == '~')){
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

    // np représente le prochain chemin, celui que l'utilisateur demande
    char* np = calloc(MAX_STRING_LENGTH, sizeof(char));

    if (abs){
        // Si la query est un chemin absolu
        strcat(np, query + 3);
    } else {
        // Sinon, on concatène avec le chemin actuel
        strcat(strcat(strcat(np, cur), "/"), query + 3);
    }

    struct stat sb;
    if (stat(np, &sb) == 0){
        if ((sb.st_mode & S_IFMT) == S_IFDIR){
        // If the path points to an existing directory
        // effectively change the cwd
        strcpy(last, cur);
        strcpy(cur, query + 3);

        char* temp = calloc(MAX_STRING_LENGTH, sizeof(char));
        if (realpath(np, temp) == NULL){
            if (abs){write(1, "r", 1);}
            write(1, "trouvé le pb", strlen("trouvé le pb"));
        }
        strcpy(cur, temp);

        free(np);
        free(temp);
        return 0;
        } else {
            write(1, "cd: ", 4);
            write(1, query+3, strlen(query)-3);
            write(1, " : Not a directory\n", strlen(" : Not a directory\n"));
        }
    } else {
        write(1, "cd: ", 4);
        write(1, query+3, strlen(query)-3);
        write(1, " : No such file or directory\n", strlen(" : No such file or directory\n"));
    }

    free(np);
    return 1;
}