#include "utils.h"

int my_kill(int argc, char** argv, job_list* jobs){
    int sig; char* ptet_pid; pid_t pid;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "kill: invalid number of arguments\n");
        tuto:
        fprintf(stderr, "use: kill [-SIG] (%cjobID|pid)\n", '%');
        return 1;
    } if (argc == 2) {
        // Si aucun signal n'est fourni
        sig = SIGTERM;
        ptet_pid = argv[1];
    } else {
        if (argv[1][0] != '-' || !is_numerical(argv[1]+1)){goto tuto;}
        sig = atoi(argv[1]+1);
        ptet_pid = argv[2];
    }

    if (ptet_pid[0] == '%'){
        // Dans le cas où on vise un job
        if (!is_numerical(ptet_pid+1)){goto tuto;}
        int v = atoi(ptet_pid+1);
        job_node* acc = jobs->head;
        while (acc && acc->jid != v){
            acc = acc->next;
        } if (!acc) {
            // On a pas trouvé le job dans la liste
            fprintf(stderr, "requested job does not exist\n");
            return 1;
        } pid = acc->pid;
    } else {
        // Dans le cas où on vise un process
        if (!is_numerical(ptet_pid)){goto tuto;}
        pid = atoi(ptet_pid);
    }

    if (kill(-pid, sig) < 0){
        perror("kill");
        return 1;
    }

    return 0;
}