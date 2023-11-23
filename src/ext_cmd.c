#include "lib.h"

int execute_ext_cmd(char **query) {

    pid_t pid = fork();

    if (pid == 0) {
        // Processus fils
	    execvp(query[0],query);
        perror("Erreur lors de l'exécution de la commande");//si execvp s'est bien déroulé bien, on atteint pas ce perror
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Erreur lors de la création du processus fils");
        return -1; // indique une erreur
    } else {
        int st; //info sur l'état de sortie du processus pid
        waitpid(pid, &st, 0);

        if (WIFEXITED(st)) {
            // vraie si la commande externe s'est terminée correctement
            return WEXITSTATUS(st); // et renvoie le code de sortie du processus
        } else {
            // La commande ne s'est pas terminée normalement
            return -1; // donc on renvoie une valeur indiquant une erreur
        }
    }
}
