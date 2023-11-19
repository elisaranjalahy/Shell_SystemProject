#define MAX_STRING_LENGTH 1 << 10


//////
//
//////

/**
 * String to unsigned int
 *
 * @param str a pointer to the start of the string
 * @return the integer represented by the string
 */
unsigned int stou(char* str);

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
    int jobID;
    int pgid; // Process Group ID
    char command[MAX_STRING_LENGTH];
    int state;
    int background;
    struct JobNode* next;
} job_node;

typedef struct JobList {
    int length;
    job_node* head;
    job_node* tail;
} job_list;

job_node* new_job_node();
job_list* new_job_list();