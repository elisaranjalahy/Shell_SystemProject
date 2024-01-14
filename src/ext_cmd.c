#include "utils.h"

void run_command(int argc, char** argv){
    if (!argc) exit(1);
    if (!strcmp(argv[0], "pwd")) exit(pwd(getenv("PWD")));
    execvp(argv[0], argv);
}

int execute_ext_cmd(char **query, job_list* jobs, int fg) {
    pid_t pid = fork();
    if (pid == 0) {
        setup_signals(SIG_DFL);         // Rétablissement des signaux
        setpgid(0, getpid());           // Construction d'un groupe de process propre

        query = parse_substitut(query);
        query = parse_pipes(query);

        if (redirections(query)){
            for(int i = 0; query[i]; free(query[i++]));
            free(query);
            exit(EXIT_FAILURE);
        }

        run_command(argvlen(query), query); // Exécution de la commande

        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
	    return 1;
    } else {
        int st = 0; int npid;               //info sur l'état de sortie du processus pid

        job_node* job = new_job_node(query, pid, "Running", next_job_id(jobs), fg);
        add_job_to_list(jobs, job);

        // On attend pas les process en arrière plan.
        if (!fg) {print_job(job, stderr); return 0;}

        tcsetpgrp(STDIN_FILENO, pid);


        // Boucle d'attente de fin du processus à l'avant plan
        // Et annonce des jobs qui finissent entre temps.
        while ((npid = waitpid(-1, &st, WUNTRACED | WCONTINUED)) != pid) {
            update_job(npid, st, jobs, stderr);
        }

        // On rend à jsh le contrôle du shell
        tcsetpgrp(STDIN_FILENO, getpid());
        update_job(pid, st, jobs, stderr);

        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st);
        } else if (WIFSIGNALED(st)) {
            return WTERMSIG(st) + 128;
        } else {
            return 1;
        }
    }
}
