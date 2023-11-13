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
 * Returns a path of the form ../../..
 * that goes from the executable to home
 *
 * @param jsh_loc the path to the jsh executable
 * @return the path back to home
 */
char* get_home_location(char* jsh_loc);

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