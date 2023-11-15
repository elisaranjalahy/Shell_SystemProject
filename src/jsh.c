#define _XOPEN_SOURCE 700 // C'est de la magie noire.

#include "my_cd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <sys/stat.h>

char* mkprompt(job_list* jobs, char* cur_path){

    // Allocation de l'espace stockant le prompt
    char* prompt = calloc(MAX_STRING_LENGTH, sizeof(char));

    // Création de la première partie du prompt
    // le nombre de jobs, en rouge et en gras
    char* njob = utos(jobs->length);    // char* du nombre de jobs en cours
    strcat(strcat(strcat(prompt, "\001\033[31;1m\002["), njob), "]\001\033[36;22m\002");
    free(njob);

    // Création de la deuxième partie du prompt
    // le chemin actuel, eventuellement tronqué
    // On veut une longueur d'au plus 28 afin de rajouter
    // `$ ` à la fin sans dépasser 30 de longueur
    int l = my_strlen(prompt) + my_strlen(cur_path) - 28;
    if (l>0) {
        // Si le chemin est trop long on le fait commencer
        // par des points de suspension
        strcat(prompt, "...");
        strcat(prompt, cur_path + l + 3);
    } else {
        strcat(prompt, cur_path);
    }

    // Finalisation du prompt
    return strcat(prompt, "\001\033[0m\002$ ");
}

int main(){
    job_list* jobs = new_job_list();
    rl_outstream = stderr;  // Affichage du prompt sur la sortie erreur

    int last_cmd_success;
    int qlength;

    // Boucle lisant l'entrée utilisateur.
    for (;;){
        char* prompt = mkprompt(jobs, getenv("PWD"));
        char* query = readline(prompt);

        free(prompt);
        add_history(query);

        // Façon temporaire de reconnaitre les commandes
        qlength = strlen(query);

        if (qlength > 1 && query[0] == 'c' && query[1] == 'd' && (qlength == 2 || query[2] == ' ')){
            // Si la commande de l'utilisateur est `cd`
            // Si possible, simplifier ce `if` dans le futur, il est horrible
            last_cmd_success = my_cd(getenv("PWD"), query);
        }else if(qlength ==1 && query[0] == '?'){
            if(last_cmd_success!=0){
                write(1, "1",1);
                write(1,"\n",1);
            }else{
                write(1,"0",1);
                write(1,"\n",1);
            }
        }else{
            last_cmd_success = 1;
        }

        if (query){free(query);}
    }

    return 0;
}