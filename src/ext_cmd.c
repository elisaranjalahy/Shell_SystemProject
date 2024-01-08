#include "utils.h"

int execute_ext_cmd(char **query, job_list* jobs) {

    pid_t pid = fork();
    if (pid == 0) {
        // Processus fils
        setup_signals(SIG_DFL);

        execvp(query[0],query);

        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
	    return 1;//tab;
    } else {
        int st; int npid; //info sur l'état de sortie du processus pid

        job_node* job = new_job_node(query, pid, "Running", next_job_id(jobs), 1);
        add_job_to_list(jobs, job);

        bac:
        if ((npid = waitpid(-1, &st, WUNTRACED | WCONTINUED)) != pid){
            update_job(npid, st, jobs, stderr);
            goto bac;
        }

        update_job(pid, st, jobs, stderr);
        if (!kill(pid, 0) && jobs->main_pid != getpid()){goto bac;}
        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st);
        } else {
            return 1;
        }
    }
}