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


//à commenter
job_node* new_job_node();
job_list* new_job_list();
int affiche_jobs(job_list* jobs);
void add_job_to_list(job_list* jobList, job_node* jobs);