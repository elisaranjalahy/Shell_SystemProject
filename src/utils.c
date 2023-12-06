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

job_node* new_job_node(char **query){
    job_node* newJob = malloc(sizeof(job_node));
   if (newJob == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour newJob\n");
        return NULL;
    }

    newJob->pgid = getgid(); //prend le pid du groupe de process appelant donc des qu'on fait un fork, apres appele new_job node pour s'ajouter lui meme à la lsit des job ?

    strcpy(newJob->command, query[0]);
    if (query[1] != NULL) {
        strncat(newJob->command, " ", PATH_MAX - strlen(newJob->command) - 1);
        strncat(newJob->command, query[1], PATH_MAX - strlen(newJob->command) - 1);
    }

    //etat à definir
    //background à definir
    newJob->next=NULL;
    return newJob;
}


int affiche_jobs(job_list* jobs){
    job_node* acc = jobs->head;//pour parcourir la liste sans changer le vrai pointeur head
    int jobID=0;
    while (acc != NULL) {

        printf("[%d] %d (état) %s\n", jobID, getgid(), acc->command);
        acc = acc->next;
        ++jobID;
    }
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

