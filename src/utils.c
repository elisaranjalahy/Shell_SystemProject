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

job_node* new_job_node(char **query, pid_t pid, char* st, int jid, int fg){
    job_node* newJob = malloc(sizeof(job_node));
   if (newJob == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour newJob\n");
        return NULL;
    }

    newJob->pid = pid;
    newJob->jid = jid;
    newJob->fg = fg;

    int i = 0;
    newJob->command[0] = '\0';
    strcat(newJob->command, query[0]);
    while (query[++i] != NULL) {
        strncat(newJob->command, " ", PATH_MAX - strlen(newJob->command) - 1);
        strncat(newJob->command, query[i], PATH_MAX - strlen(newJob->command) - 1);
    }
    newJob->state=st;
    newJob->next=NULL;
    return newJob;
}

int affiche_jobs(job_list* jobs){
    job_node* acc = jobs->head;//pour parcourir la liste sans changer le vrai pointeur head
    maj_etat_jobs(jobs);
    while (acc != NULL){ //&& acc->state[0] == 'R') {
        if (acc->jid > 0){
            printf("[%d]\t%d\t%s\t%s\n", acc->jid, acc->pid,acc->state, acc->command);
        }
        if (acc->state[0] < 'L'){acc->jid = -1;}
        acc = acc->next;
    }
    return 0;
}

void add_job_to_list(job_list* jobList, job_node* job){
    if(job !=NULL){
        if (jobList->head ==NULL){
            jobList->head=job;
            jobList->tail=job;
        }else{
            jobList->tail->next=job;
            jobList->tail=job;
        }
        jobList->length++;
        if (!job->fg) fprintf(stderr, "[%d]\t%d\t%s\t%s\n", job->jid, (int)job->pid, job->state, job->command);
    }
}

void maj_etat_jobs(job_list* jobs) {
    job_node* acc = jobs->head;
    int st;

    while (acc != NULL) {

        if (acc->jid > 0 && acc->state[0] >= 'R'){
            int val = waitpid(acc->pid, &st, WNOHANG | WCONTINUED);

            if (val == -1) {
                perror("Erreur lors de l'appel à waitpid");
                exit(EXIT_FAILURE);
            } else if (val){
                if (WIFEXITED(st)) {
                    acc->state="Done";
                    jobs->length--;
                } else if (WIFSIGNALED(st)) {
                    acc->state="Killed";
                    jobs->length--;
                } else if (WIFSTOPPED(st)) {
                    acc->state="Stopped";
                } else if (WIFCONTINUED(st)) {
                    acc->state="Running";
                } else {
                    perror("waitpid");
                }
            }
        }
        acc = acc->next;
    }
}

void update_job(pid_t pid, int st, job_list* jobs, FILE* output){
    job_node* acc = jobs->head;
    int i = 1;
    while (acc != NULL){
        if (acc->pid == pid){
            if (WIFEXITED(st)) {
                acc->state="Done";
            } else if (WIFSIGNALED(st)) {
                acc->state="Killed";
            } else if (WIFSTOPPED(st)) {
                acc->state="Stopped";
                jobs->length ++;
            } else if (WIFCONTINUED(st)) {
                acc->state="Running";
                jobs->length ++;
            } else {
                perror("waitpid");
            }
            if (!acc->fg || acc->state[0] >= 'K') fprintf(output, "[%d]\t%d\t%s\t%s\n", acc->jid, (int)acc->pid, acc->state, acc->command);
            if (acc->state[0] < 'R') acc->jid = -1;
            jobs->length--;
            break;
        }
        i++;
        if (acc){acc = acc->next;}
    }
}

int next_job_id(job_list* job_list){
    int tor = 0; int tmp = 1;
    job_node* job = job_list->head;

    while (tmp != tor){
        tor = tmp;
        while (job){
            if (job->jid == tmp) tmp++;
            job = job->next;
        }
    }
    return tor;
}

//////
//          exit
//////

bool exit_possible(job_list* jobs){
    job_node* acc = jobs->head;
    while (acc != NULL){
        if(strcmp(acc->state,"Running")==0||strcmp(acc->state,"Stopped")==0){
            return false;
        }
        acc = acc->next;
    }
    return true;
}