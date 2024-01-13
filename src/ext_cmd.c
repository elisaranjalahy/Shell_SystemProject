#include "utils.h"

int execute_ext_cmd(char **query, job_list* jobs, int fg) {
    jobs->length++;
    setup_signals(SIG_DFL);
    pid_t pid = fork();
    if (pid == 0) {
        // Processus fils
        setup_signals(SIG_DFL);
        setpgid(0, getpid());

        execvp(query[0],query);

        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
	    return 1;//tab;
    } else {
        setup_signals(SIG_IGN);
        int st = 0; int npid; //info sur l'état de sortie du processus pid

        jobs->tail->pid = pid;
        if (!jobs->tail->fg) print_job(jobs->tail, stderr);

        bac:
        if (fg && (npid = waitpid(-1, &st, WUNTRACED | WCONTINUED)) != pid){
            update_job(npid, st, jobs, stderr);
            goto bac;
        }

        if (fg) update_job(pid, st, jobs, stderr);
        if (!kill(pid, 0) && jobs->main_pid != getpid()){goto bac;}
        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st);
        } else {
            return 1;
        }
    }
}