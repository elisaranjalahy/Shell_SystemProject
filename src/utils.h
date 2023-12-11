#include "lib.h"

#ifndef PATH_MAX
    #define PATH_MAX (1 << 10)
#endif

////
//  Fonctions utiliaires
////

/**
 * Unsigned int to string
 *
 * @param n an unsigned integer
 * @return the string representation of n
 */
char* utos(unsigned int n);

/**
 * Returns the length of a string ignoring
 * the utilities characters.
 *
 * @param str an unsigned integer
 * @return the printed length of str
 */
int my_strlen(const char* str);

/**
 * Transforme l'entrée en un char* à la
 * façon de l'argument argv du main usuel.
 *
 * @param query le char* à transformer
 * @return l'entrée transformée, sans espaces
 */
char** my_to_argv(char* query);

/**
 * Renvoie la longueur d'un array de strings
 *
 * @param argv un char** se terminant par un pointeur NULL
 * @return la longueur de argv
 */
int argvlen(char** argv);

//////
//          Jobs
//////

typedef struct JobNode {
    int pgid; // Process Group ID
    char command[PATH_MAX];
    char state[PATH_MAX];
    int background;
    struct JobNode* next;
} job_node;

typedef struct JobList {
    int length;
    job_node* head;
    job_node* tail;
} job_list;


/**
 * initialise et renvoie un nouveau job à chaque commande
 * externe appelée en arrière-plan
 * definit son champ @param pgid
 * récupere la commande appelée pour definir @param command
 * @param state definit son état actuel (running, done...)
 * definit @param next à NULL
 **/
job_node* new_job_node();

/**
 * Initialise une nouvelle liste de jobs
 **/
job_list* new_job_list();

/**
 * affiche la liste @param jobs de jobs donnée en argument
 **/
int affiche_jobs(job_list* jobs);

/**
 * ajoute le le nouveau jobs @param jobs
 * à la liste de jobs @param jobList
 **/
void add_job_to_list(job_list* jobList, job_node* jobs);


//à commenter
void mkrdr(int new_fd, const char* filename, int flags, mode_t mode);
int mkflags(char* rdr);
void redirections(char** argv);