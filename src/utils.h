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

typedef struct CommandResults {
    gid_t pid; //stock l'id de groupe 
    int status; // 0 si réussi, -1 sinon
    char* state;// Running, Done, Stopped, Killed ou Dettached
} command_results;

/**
 * Permet l'execution des commande externe par jsh
 * @param args prend la commande externe et ses arguments/options donnés au shell
 * @return 0 si cette commande s'est exécutée correctement, sinon une autre valeur indiquant une erreur
 */
command_results execute_ext_cmd(char **args);




//////
//          Jobs
//////

typedef struct JobNode {
    int pid; // Process Group ID
    char command[PATH_MAX];
    char* state;//Running, Done, Stopped, Killed ou Dettached
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
 * definit son champ @param pid
 * récupere la commande appelée pour definir @param command
 * @param state definit son état actuel (running, done...)
 * definit @param next à NULL
 **/
job_node* new_job_node(char **query, pid_t pid,char* st);

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

void maj_etat_jobs(job_list* job_list);


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
