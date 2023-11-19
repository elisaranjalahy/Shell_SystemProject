#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/wait.h>

/**
* Affiche le chemin absolue du répertoire de travail courant
 * @param cur the current path
 **/
 int pwd(char* cur);

 /**
 * Change le répertoire courant si directory est valide.
 * @param cur le chemin courant
 * @param directory la recherche de l'utilisateur
 * @return 1 en cas d'erreur, sinon 0
 */
int my_cd(char* cur, char** directory);

/**
 * Permet l'execution des commande externe par jsh
 * @param args prend la commande externe et ses arguments/options donnés au shell
 * @return 0 si cette commande s'est exécutée correctement, sinon une autre valeur indiquant une erreur
 */
int execute_ext_cmd(char **args);