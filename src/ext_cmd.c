#include "utils.h"

int execute_ext_cmd(char **query, job_list* jobs) {

    //command_results tab; //tab[0 contient la valeur de succes de l'appel, tab[1] contient le pid

    pid_t pid = fork();
    if (pid == 0) {
        // Processus fils
        //redirections(query);
        execvp(query[0],query);
        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
	    return 1;//tab;
    } else {
        int st; int npid; //info sur l'état de sortie du processus pid

        bac:
        if ((npid = waitpid(-1, &st, 0)) != pid){
            update_job(npid, st, jobs);
            goto bac;
        }

        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st); // et renvoie le code de sortie du processus
            //tab.pid = pid;
            //return tab;
        } else {
            // La commande ne s'est pas terminée normalement
            //tab.status = 1; // indique une erreur
            return 1;
        }
    }

}