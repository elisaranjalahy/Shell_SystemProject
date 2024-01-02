#include "utils.h"

int my_kill(int argc, char** argv, job_list* jobs){
    int sig; char* ptet_pid; pid_t pid;
    if (argc < 1 || argc > 3) {
        fprintf(stderr, "kill: invalid number of arguments\n");
        return 1;
    } if (argc == 2) {
        sig = SIGTERM;
        ptet_pid = argv[1];
    } else {
        sig = atoi(argv[1]+1);
        ptet_pid = argv[2];
    }

    if (ptet_pid[0] == '%'){
        int v = atoi(ptet_pid+1);
        job_node* acc = jobs->head;
        while (acc && acc->jid != v){
            acc = acc->next;
        } if (!acc) {
            fprintf(stderr, "requested job does not exist\n");
            return 1;
        } pid = acc->pid;
    } else {
        pid = atoi(ptet_pid);
    }

    //fprintf(stderr, "killing pid : %d\n", pid);
    //sleep(2);
    if (kill(-pid, sig) < 0){
        perror("kill");
        return 1;
    }

    //fprintf(stdout, "kill: sent signal %d to process %d\n", sig, pid);
    //maj_etat_jobs(jobs);
    return 0;
}