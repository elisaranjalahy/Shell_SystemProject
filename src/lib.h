#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>


/**
 * Ce fichier permet d'acceder aux signatures de certaines commandes ou encore de reunir toutes les bibliotheques
 * utilisées pour n'avoir qu'un seul "include" à écrire dans les autres fichiers
 **/

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






