#include "utils.h"


//////
//          Misc
//////

char* utos(unsigned int n){
    int decimals = (n == 0);
    for(int m=n; m > 0; m /= 10){decimals++;}
    // unsigned int to string
    char* res = malloc(sizeof(char)*(decimals+1));
    if (res == NULL){return NULL;}

    int i = decimals;
    for(; n > 0 || i > 0; n /= 10){
        res[--i] = (char)((n % 10)+48);
    }
    res[decimals] = '\0';
    return res;
}

int my_strlen(const char* str){
    // Careful, this should only be used on properly formatted string.
    int j = 0;
    for(int i=0;;i++){
        if (str[i] == '\0'){return i-j;}
        if (str[i] == '\001'){j++;do {i++;j++;} while(str[i] != '\002' && str[i] != '\0');}
    }
}

int argvlen(char** argv){
    int i = -1;
    while (argv[++i] != NULL);
    return i;
}

char** my_to_argv(char* query){
    int i = 0;
    char** argv = calloc(strlen(query) + 1, sizeof(char*));

    char* arg = strtok(query, " ");

    while (arg != NULL){
        argv[i] = calloc(strlen(arg) + 1, sizeof(char));
        if (argv[i] == NULL){goto error;}
        strcpy(argv[i++], arg);
        arg = strtok(NULL, " ");
    }
    for (;i<strlen(query) + 1; argv[i++] = NULL);
    return argv;

    error:
    for(; i>0; free(argv[--i]));
    free(argv);
    perror("my_to_argv");
    return NULL;
}

//////
//          Jobs
//////

job_list* new_job_list(){
    job_list* list = malloc(sizeof(job_list));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

job_node* new_job_node(char **query,pid_t pid,char* st){
    job_node* newJob = malloc(sizeof(job_node));
   if (newJob == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour newJob\n");
        return NULL;
    }

    newJob->pid = pid;
    strcpy(newJob->command, query[0]);
    if (query[1] != NULL) {
        strncat(newJob->command, " ", PATH_MAX - strlen(newJob->command) - 1);
        strncat(newJob->command, query[1], PATH_MAX - strlen(newJob->command) - 1);
    }
    newJob->state=st;
    newJob->next=NULL;
    return newJob;
}


int affiche_jobs(job_list* jobs){
    job_node* acc = jobs->head;//pour parcourir la liste sans changer le vrai pointeur head
    int jobID=0;
    while (acc != NULL) {

        printf("[%d] %d %s %s\n", jobID, acc->pid,acc->state, acc->command);
        acc = acc->next;
        ++jobID;
    }
    return 0;
}

void add_job_to_list(job_list* jobList, job_node* jobs){
    if(jobs !=NULL){
        if (jobList->head ==NULL){
            jobList->head=jobs;
            jobList->tail=jobs;
        }else{
            jobList->tail->next=jobs;
            jobList->tail=jobs;
        }
    }
}

void maj_etat_jobs(job_list* job_list) {
    job_node* acc = job_list->head;
    int st;

    while (acc != NULL) {
        if(getpid()!=0){
            int val = waitpid(acc->pid, &st, WNOHANG | WUNTRACED);

            if (val == -1) {
                perror("Erreur lors de l'appel à waitpid");
                exit(EXIT_FAILURE);
            } else if (val == 0) {
                // Le processus n'a pas encore changé d'état
                acc = acc->next;
            } else {
                if (WIFEXITED(st)) {
                    acc->state="Done";
                } else if (WIFSIGNALED(st)) {
                    acc->state="Killed";
                } else if (WIFSTOPPED(st)) {
                    acc->state="Stopped";
                }
                acc = acc->next;
            }
        }
    }
}


//////
//          Redirection
//////

int mkrdr(int new_fd, const char* filename, int flags, mode_t mode){
    int fd;
    if ((fd = open(filename, flags, mode)) < 0){
        perror("open");
        return 1;
    }
    if (dup2(fd, new_fd) != new_fd){
        perror("dup2");
        return 1;
    }
    close(fd);
    return 0;
}

int mkflags(char* rdr){
    if (rdr[0] == '2'){rdr++;}
    if (rdr[0] == '>'){
        int flags = O_WRONLY | O_CREAT;
        if (rdr[1] == '\0'){return flags | O_EXCL;}
        else if (rdr[1] == '>' && rdr[2] == '\0'){return flags | O_APPEND;}
        else if (rdr[1] == '|' && rdr[2] == '\0'){return flags | O_TRUNC;}
    }
    return 0;
}

int redirections(char** argv){
    int incr = 1;
    for (int i = 0; i < argvlen(argv) - 1; i += incr){
        incr = 1;

        if (argv[i][0] == '>'){
            int flags = mkflags(argv[i]);
            if (!flags){continue;}

            if (mkrdr(STDOUT_FILENO, argv[i+1], flags, 420)){return 1;}
            incr = 0;
        } else if (argv[i][0] == '2'){
            int flags = mkflags(argv[i]);
            if (!flags){continue;}

            if (mkrdr(2, argv[i+1], flags, 420)){return 1;}
            incr = 0;
        } else if (argv[i][0] == '<' && argv[i][1] == '\0'){
            int fd;
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                perror("open");
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) != STDIN_FILENO){
                perror("dup2");
                return 1;
            }
            incr = 0;

        }

        if (incr == 0){
            int len = argvlen(argv);
            free(argv[i]); free(argv[i+1]);
            for(int k = i; k < len - 2; k++){
                argv[k] = argv[k+2];
            }
            argv[len-1] = NULL; argv[len-2] = NULL;
        }
    }
    return 0;
}
