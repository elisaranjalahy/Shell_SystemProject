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
#include <stdbool.h>
#include <dirent.h>


#ifndef PATH_MAX
    #define PATH_MAX (1 << 10)
#endif


typedef struct JobNode {
    int jid; // Job ID
    int fg;
    int pid; // Process ID
    char command[PATH_MAX];
    char* state;//Running, Done, Stopped, Killed ou Detached
    struct JobNode* next;
} job_node;

typedef struct JobList {
    int length;
    int main_pid;
    job_node* head;
    job_node* tail;
} job_list;


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


//////
//          Redirections
//////

 /**
 * Effectue la redirection en pratique
 * @param new_fd le flux à rediriger
 * @param filmename le fichier où rediriger le flux
 * @param flags  et @param mode les options d'open()
 */
int mkrdr(int new_fd, const char* filename, int flags, mode_t mode);

/**
 * Détermine les flags de open() en fonction
 * de la redirection souhaitée
 * @param rdr le string de la redirection souhaitée
 * @return les flags.
 */
int mkflags(char* rdr);

/**
 * Parcours l'entrée utilisateur à la recherche de redirection
 * à effectuer, le cas échéant utilise les fonctions ci-dessus.
 * @param argv l'entrée utilisateur en format plus simple à parcourir.
 */
int redirections(char** argv);

int my_kill(int argc, char** argv, job_list* jobs);



