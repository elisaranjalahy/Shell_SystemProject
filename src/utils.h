#include "lib.h"

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
 * Créent des sous-process chacun avec le bon
 * @param argv et les bonnes redirections
 */
char** parse_pipes(char** argv);
char** parse_substitut(char** argv);

/**
 * Récupere un pid donné au prompt comme parametre d'une commande
**/
int getArgPid (int pid);



//////
//          Jobs
//////



/**
 * Permet l'execution des commande externe par jsh
 * @param args prend la commande externe et ses arguments/options donnés au shell
 * @return 0 si cette commande s'est exécutée correctement, sinon une autre valeur indiquant une erreur
 */
int execute_ext_cmd(char **args, job_list* jobs);

/**
 * initialise et renvoie un nouveau job à chaque commande
 * externe appelée en arrière-plan
 * definit son champ @param pid
 * récupere la commande appelée pour definir @param command
 * @param state definit son état actuel (running, done...)
 * definit @param next à NULL
 **/
job_node* new_job_node(char **query, pid_t pid, char* st, int jid, int fg);

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

/**
 * Met à jour les états des jobs de la liste @param job_list
**/
void maj_etat_jobs(job_list* job_list);

/**
 * Met à jour l'un job spécifique de la liste @param jobs
 * @param pid le pid de ce job
 * @param st son état actuel
**/
void update_job(pid_t pid, int st, job_list* jobs, FILE* output);

/**
 * permet de définir correctement les @param jid de chaque jobs dans la liste @param job_list
 **/
int next_job_id(job_list* job_list);

/**
 * Recupere la commande associé au job de pid @param pid
**/
const char* getCommand(job_list *jobList, int jid);


//////
//          exit
//////
bool exit_possible(job_list* jobLits);