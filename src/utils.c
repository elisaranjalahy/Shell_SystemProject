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

int is_numerical(char* str){
    for(int i = 0; *(str+i); i++){
        if (i[str] < 48 || i[str] > 57){return 0;}
    }
    return 1;
}

void setup_signals(void(*fun)(int)){
    struct sigaction sa = {0};
    sa.sa_handler = fun;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGTTIN, &sa, NULL);
    sigaction(SIGTTOU, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

//////
//          Jobs
//////

job_list* new_job_list(){
    job_list* list = malloc(sizeof(job_list));
    list->length = 0;
    list->main_pid = getpid();
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void purge_job_list(job_list* jobs){
    job_node* job = jobs->head;
    while (job && job->jid <= 0){
        jobs->head = job->next;
        free(job);
        job = jobs->head;
    }
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

void hotfix_pipelines_background(char* command, int add){
    size_t len = strlen(command);
    for (int i=0; command[i]; i++){
        if (command[i] == '('){
            if (command[i+1] != ' ' && add){
                command[len+1] = '\0';
                for(int j = len; j >= i; command[j--+1] = command[j]);
                command[i+1] = ' ';
                len = strlen(command);
            } else if (command[i+1] == ' ' && !add){
                for(int j = i+1; j <= len; command[j++] = command[j+1]);
                command[len-1] = '\0';
                len = strlen(command);
            }
        } else if (command[i] == ')'){
            if (command[i-1] != ' ' && add){
                command[len+1] = '\0';
                for(int j = len+1; j >= i; command[j--+1] = command[j]);
                command[i] = ' ';
                len = strlen(command);
            } else if (command[i-1] == ' ' && !add){
                for(int j = i-1; j <= len; command[j++] = command[j+1]);
                command[len-1] = '\0';
                len = strlen(command);
            }
        }
    }
}

void print_job(job_node* job, FILE* out){
    /* Hotfix des espaces dans les substitutions pour le test pipeline-background
    if (job->state[0] == 'R'){hotfix_pipelines_background(job->command, 1);}
    else if (job->state[0] == 'S'){hotfix_pipelines_background(job->command, 0);}*/
    char* s = "";
    if (job->fg){s = "\n";}
    fprintf(out, "%s[%d]\t%d\t%s\t%s\n", s, job->jid, job->pid, job->state, job->command);
}

int affiche_jobs(job_list* jobs, int debug){
    job_node* acc = jobs->head;//pour parcourir la liste sans changer le vrai pointeur head
    maj_etat_jobs(jobs);
    while (acc != NULL){
        if ((acc->jid > 0 && acc->pid) || debug){
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

    }
}

void maj_etat_jobs(job_list* jobs) {
    job_node* acc = jobs->head;
    int st;

    while (acc != NULL) {
        //if (!acc->pid){acc = acc->next; continue;}
        if (acc->pid > 0 && acc->jid > 0 && acc->state[0] >= 'R'){
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
    int k = 1 - kill(-pid, 0);
    // k = 1 si le job a au moints un fils actif, 0 sinon
    while (acc != NULL){
        if (acc->pid == 0 || acc->pid == pid){
            if (WIFEXITED(st) && k) {
                acc->state="Done";
            } else if (WIFSIGNALED(st)) {
                acc->state="Killed";
            } else if (WIFSTOPPED(st)) {
                acc->state="Stopped";
                if (acc->pid) jobs->length ++;
            } else if (WIFCONTINUED(st)) {
                acc->state="Running";
                if (acc->pid) jobs->length ++;
            } else if (k) {
                acc->state="Detached";
            } else {
                perror("waitpid");
            }
            if (!acc->fg || acc->state[0] == 'S') print_job(acc, stderr); // Job en arrire plan fini, ou job en avant plan stoppé
            else if (acc->state[0] == 'K' && WTERMSIG(st) == SIGTERM) fprintf(stderr, "Terminated\n");
            acc->fg = 0;

            if (acc->state[0] < 'R') acc->jid = -1;
            if (acc->pid){
                jobs->length--;
                break;
            }
        }
        acc = acc->next;
    }
}

int next_job_id(job_list* job_list){
    int tmp = 1;
    job_node* job = job_list->head;

    while (job){
        if (job->jid == tmp) {
            tmp++;
            job = job_list->head;
        } else {
            job = job->next;
        }
    }
    return tmp;
}

job_node* getJob(int jobPid,job_list *jobs){
    job_node* acc = jobs->head;
    while (acc != NULL){
        if (acc->pid == jobPid){
            return acc;
        }
        acc = acc->next;
    }
    return NULL;
}

int getArgJid (char ** argv){
    int jobJid;

    if (sscanf(argv[1], "%%%d", &jobJid) != 1) {
        fprintf(stderr, "Erreur lors de l'extraction du pid\n");
        return 1;
    }
    return jobJid;
}

int getPid(int jid,job_list *jobs){
    job_node* acc = jobs->head;
    while (acc != NULL){
        if (acc->jid == jid){
            return acc->pid;
        }
        acc = acc->next;
    }
    perror ("Ce PID n'existe pas");
    return 1;
}

char* getState(char* s){
    if(strcmp(s,"R")==0){
        return "Running";
    }
    if(strcmp(s,"T")==0){
        return "Stopped";
    }
    if(strcmp(s,"S")==0){
        return "Running";
    }
    if(strcmp(s,"K")==0){
        return "Killed";
    }
    return "Done";
}

void affiche_Jobs_arbo(char* jsh_pidString, job_list* jobs,int tab){ // tab pour le calcul de l'arbre
    DIR *dir;
    struct dirent *d;
    dir = opendir("/proc");
    if (dir != NULL) {
        while ((d = readdir(dir)) && d != NULL) {
            if (atoi(d->d_name) > 0) { //conversion car nom des repertoire sont des string
                char path[269];
                snprintf(path, sizeof(path), "/proc/%s/status", d->d_name); //acceder aux info du fichier
                FILE *file = fopen(path, "r");//ouverture du fichier en lecture
                if (file == NULL) {
                    perror("erreur à l'ouverture");
                    exit(1);
                }
                if (file != NULL) {
                    char line[256];
                    char name[256] = "";
                    char pid[256] = "";
                    char state[256] = "";
		            char ppid[256]="";
                    while (fgets(line, sizeof(line), file)) {
                        if (strncmp(line, "Name:", 5) == 0) {
                            sscanf(line, "Name:\t%s", name);
                        } else if (strncmp(line, "Pid:", 4) == 0) {
                            sscanf(line, "Pid:\t%s", pid);
                        } else if (strncmp(line, "State:", 6) == 0) {
                            sscanf(line, "State:\t%s", state);
                        } else if (strncmp(line, "PPid:",5) == 0){
			                    sscanf(line,"PPid:\t%s",ppid);
			            }
                    }

                    fclose(file);
                    if (strcmp(ppid, jsh_pidString) == 0){
			            job_node * j = getJob(atoi(pid),jobs);
			            int n=0;
			            if(j!=NULL){
				            n=j->jid;
                            printf("[%d]\t%s\t%s\t%s\n",n, pid, getState(state), name);
			            } else{
				            n=tab;
                            printf("\t%s\t%s\t%s\n", pid, getState(state), name);
			            }
			            affiche_Jobs_arbo(pid,jobs,n);
                        
                    }
                }

            }
            
        }
    }
    closedir(dir);
}


//////
//          exit
//////

bool exit_possible(job_list* jobs){
    return (jobs->length == 0);
}

bool is_valid_syntax(char* str){
    return (str && (str[0] == '.' || str[0] == '/' || str[0] == '<' ||(str[0] >= 'A' && str[0] <= 'Z') || (str[0] >= 'a' && str[0] <= 'z')));
}

int parse_erreur_syntaxe(int argc, char** argv){
    int i = 0; int d = 0;
    for (; i < argc; i++){
        if (argv[i][0] == '>' && !is_valid_syntax(argv[i+1])){
            // Redirection de la sortie standard non valide.
            goto syntax_error;
        } else if (argv[i][0] == '2' && argv[i][1] == '>' && !is_valid_syntax(argv[i+1])){
            // Redirection de la sortie erreur non valide.
            goto syntax_error;
        } else if (argv[i][0] == '|' && (!i || !argv[i+1] || argv[i+1][0] == '|')){
            // Pipe dans rien ou depuis rien
            goto syntax_error;
        } else if (!strcmp(argv[i], "<") && !is_valid_syntax(argv[i+1])){
            // Redirection de l'entrée standard non valide
            goto syntax_error;
        } else if (!strcmp(argv[i], "<(")) {
            d++;
        } else if (!strcmp(argv[i], ")")) {
            d--;
        }

    }
    if (!d){
        // d est nul sauf en cas de mauvais
        // parenthésage des substitutions
        return 0;
    }

    syntax_error: ; // Instruction vide pour eviter un warning.
    char* s = argv[i+1];
    if (!s) s = "\\n";
    fprintf(stderr, "jsh: syntax error near unexpected token: %s\n", s);
    return 1;

}

//////
//          fg
//////

int foreground(char** argv, job_list* jobs){
    int st;

    if(argv[1]!=NULL){

        int jobJid=getArgJid(argv);
        int jobPid=getPid(jobJid,jobs);

        job_node* j=getJob(jobPid,jobs);
        if(j!=NULL){
            if(strcmp(j->state,"Killed")==0){
                perror("Aucun processus en arrière-plan.\n");
                return 1;
            }

            j->fg=1;
            j->state="Running";

            tcsetpgrp(STDIN_FILENO, jobPid);//place le job à l'avant plan/comme celui controlant le terminal

            if (kill(-jobPid, SIGCONT) == -1) {
                perror("Erreur lors de l'envoi du signal SIGCONT");
                return 1;
            }

            waitpid(jobPid, &st, WUNTRACED);
            
            tcsetpgrp(STDIN_FILENO, jobs->main_pid);// réinitialise le groupe de processus de contrôle du terminal
            update_job(jobPid,st,jobs,stderr);
        }

    } else if (jobs->tail == NULL || strcmp(jobs->tail->state,"Killed")==0){
            perror("Aucun processus en arrière-plan.\n");
            return 1;

    }else{
        if(jobs->tail !=NULL){
            job_node* j=getJob(jobs->tail->pid,jobs);

            if(strcmp(j->state,"Killed")==0){
                perror("Aucun processus en arrière-plan.\n");
                return 1;
            }

            if (kill(jobs->tail->pid, SIGCONT) == -1) {
                perror("Erreur lors de l'envoi du signal SIGCONT");
                return 1;
            }

            j->fg=0;
            j->state="Running";

            tcsetpgrp(STDIN_FILENO,jobs->tail->pid);//place le job à l'avant plan/comme celui controlant le terminal
            waitpid(jobs->tail->pid, &st, WUNTRACED);
            tcsetpgrp(STDIN_FILENO, getpgrp());// réinitialise le groupe de processus de contrôle du terminal
            update_job(jobs->tail->pid,st,jobs,stderr);
        }

    }
    return 0;
}

int background(char** argv, job_list* jobs){
    if (argv[2]) {
        fprintf(stderr, "bg: Invalid number of arguments\n");
        tuto_bg:
        fprintf(stderr, "use: bg %cjobId\n", '%');
        return 1;
    }
    if (argv[1][0] != '%' || !is_numerical(1+1[argv])){goto tuto_bg;}
    int jid = atoi(1+1[argv]);
    pid_t pgid = getPid(jid, jobs);

    if (kill(-pgid, SIGCONT) < 0){
        perror("kill");
        return 1;
    }

    maj_etat_jobs(jobs);
    job_node* job = getJob(pgid, jobs);
    job->fg = 0;
    print_job(job, stderr);

    return 0;
}
