#include "utils.h"

int execute_ext_cmd(char **query, job_list* jobs, int fg) {
    jobs->length++;
    pid_t pid = fork();
    if (pid == 0) {
        setup_signals(SIG_DFL);         // Rétablissement des signaux
        setpgid(0, getpid());           // Construction d'un groupe de process propre

        execvp(query[0],query);         // Exécutiond de la commande

        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
	    return 1;
    } else {
        int st = 0; int npid;               //info sur l'état de sortie du processus pid
        jobs->tail->pid = pid;              // Affectation du pid au job
        if (!jobs->tail->fg) print_job(jobs->tail, stderr);

        // On attend pas les process en arrière plan.
        if (!fg) return 0;

        int my_pid = getpid();
        if (my_pid == jobs->main_pid) tcsetpgrp(STDIN_FILENO, pid);

        // Boucle d'attente de fin du processus à l'avant plan
        // Et annonce des jobs qui finissent entre temps.
        if (fg) {
            while ((npid = waitpid(-1, &st, WUNTRACED | WCONTINUED)) != pid) {
                update_job(npid, st, jobs, stderr);
            }

            if (my_pid == jobs->main_pid) tcsetpgrp(STDIN_FILENO, getpid());
            update_job(pid, st, jobs, stderr);
        }

        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st);
        } else if (WIFSIGNALED(st)) {
            return WTERMSIG(st) + 128;
        } else if (WIFSTOPPED(st)) {
            return 1;
        } else {
            return 1;
        }
    }
}