#include "utils.h"

command_results execute_ext_cmd(char **query) {

    command_results tab; //tab[0 contient la valeur de succes de l'appel, tab[1] contient le pid

    pid_t pid = fork();
    if (pid == 0) {
        // Processus fils
        redirections(query);
        execvp(query[0],query);
        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
        tab.status = -1; // indique une erreur
	return tab;

    } else {
        int st; //info sur l'état de sortie du processus pid
        waitpid(pid, &st, 0);

        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            tab.status = WEXITSTATUS(st); // et renvoie le code de sortie du processus
            return tab;
        } else {
            // La commande ne s'est pas terminée normalement
            tab.status = -1; // indique une erreur
            return tab;
        }
    }

}

command_results execute_ext_cmd_ap(char **query) {
    command_results tab; //tab[0 contient la valeur de succes de l'appel, tab[1] contient le pid
    tab.status=0;
    pid_t pid=fork();
   if (pid == -1) {
        perror("fork");
        tab.status=-1;
        return tab;
    } else if (pid == 0) {
        execvp(query[0],query);
        perror("Erreur lors de l'exécution de la commande");
        tab.status=-1;
        return tab;
    } else {
        
        setsid();
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);

        if (result == -1) {
            perror("waitpid");
            tab.status=-1;
            return tab;
        } else if (result == 0) {
            tab.state="Running";
            return tab;
        } else {
            if (WIFEXITED(status)) {
                tab.state="Done";
                return tab;
            } else if (WIFSIGNALED(status)) {
                tab.state="Killed";
                return tab;
            } else if (WIFSTOPPED(status)) {
                tab.state="Stopped";
                return tab;
            }
            int stopsig = WSTOPSIG(status);
            if (stopsig == SIGCONT) {
                tab.state="Running";
                return tab;
            } else {
                tab.state="Detached";
                return tab;
            }
        }
        return tab;
        
    }

}
