#define _XOPEN_SOURCE 700 // C'est de la magie noire.

#include "utils.h"
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int my_cd(const char* cur, const char* directory){
    // Les étapes ci-dessous sont celles explicités dans le manuel.
    int dirlen = strlen(directory); // Par souci d'optimisation.
    int found_file = 0;

    // Étapes 1-2
    if (dirlen == 2 || (dirlen == 4 && directory[3] == '~')){
        // Pas de requête, on tente de rejoindre le home.
        char* curpath = getenv("HOME");
        if (curpath && setenv("OLDPWD", cur, 1) == 0 && setenv("PWD", curpath, 1) == 0){
            return 0;
        }
        return 1;
    }

    if (strlen(directory) == 4 && directory[3] == '-'){
        // Go to previous directory
        char* curpath = getenv("OLDPWD");
        if (curpath && setenv("OLDPWD", cur, 1) == 0 && setenv("PWD", curpath, 1) == 0){
            write(1, curpath, strlen(curpath));
            write(1, "\n", 1);
            return 0;
        }
        return 1;
    }
    // Initialisation de curpath utile pour les prochaines étapes.
    char* curpath = calloc(MAX_STRING_LENGTH, sizeof(char));

    // Étape 3
    if (dirlen > 3 && directory[3] == '/'){
        strcpy(curpath, directory + 3);
        goto etape7;
    }

    // Étape 4
    if (dirlen > 3 && directory[3] == '.'){
        goto etape6;
    }

    // Étape 5
    char* CDPATH = calloc(MAX_STRING_LENGTH, sizeof(char));
    strcpy(CDPATH, cur);

    // Ajout du chemin actuel
    if (getenv("CDPATH")){
        strcat(CDPATH, ":");
        strcat(CDPATH, getenv("CDPATH"));
    }

    // On itère dans CDPATH
    struct stat sb;
    char *token = strtok(CDPATH, ":");

    while (token != NULL) {
        char* np = calloc(MAX_STRING_LENGTH, sizeof(char));
        if(!np){free(CDPATH);return 1;}
        strcpy(np, token);

        if (np[strlen(np)-1] != '/'){
            strcat(np, "/");
        }

        strcat(np, directory + 3);
        if (stat(np, &sb) == 0){
            if ((sb.st_mode & S_IFMT) == S_IFDIR){
                // On a trouvé le bon dossier
                strcpy(curpath, np);

                free(np);
                free(CDPATH);
                goto etape7;
            } else {
                // On a trouvé un fichier avec ce nom
                found_file = 1;
            }
        }
        if (np){free(np);}
        token = strtok(NULL, ":");
    }
    free(CDPATH);

    // Étape 6
    etape6:
    strcpy(curpath, directory + 3);

    // Étape 7
    etape7:
    if (curpath[0] != '/'){
        char* temp = calloc(MAX_STRING_LENGTH, sizeof(char));
        if(!temp){free(curpath);return 1;}
        strcat(strcat(strcat(temp, cur), "/"), curpath);
        strcpy(curpath, temp);
        free(temp);
    }

    // Étape 8-9 TODO

    // Étape 10
    if (found_file){
        free(curpath);
        write(1, "cd: ", 4);
        write(1, directory + 3, strlen(directory) - 3);
        write(1, " : Not a directory\n", strlen(" : Not a directory\n"));
        return 1;
    }

    char* _curpath = calloc(MAX_STRING_LENGTH, sizeof(char));
    if (realpath(curpath, _curpath)){
        if (setenv("OLDPWD", cur, 1) == 0 && setenv("PWD", _curpath, 1) == 0){
            free(curpath);
            free(_curpath);
            return 0;
        }
    }

    free(_curpath);
    free(curpath);
    write(1, "cd: ", 4);
    write(1, directory + 3, strlen(directory) - 3);
    write(1, " : No such file or directory\n", strlen(" : No such file or directory\n"));
    return 1;
}