#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>

/**
* Affiche le chemin absolue du répertoire de travail courant
 * @param cur the current path
 **/

 int pwd(char* cur);

 /**
 * Changes the current directory if query is valid.
 * @param cur the current path
 * @param last the previous path
 * @param query where the user wants to go
 * @return 0 on success, else 1
 */
int my_cd(char* cur, char* last, const char* query);