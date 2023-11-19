#define _XOPEN_SOURCE 700 // C'est de la magie noire.
#include "lib.h"
#include "utils.h"


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
    int argc;

    // Boucle lisant l'entrée utilisateur.
    for (;;){
        char* prompt = mkprompt(jobs, getenv("PWD"));
        char* query = readline(prompt);

        free(prompt);

        if (!query){
            continue;
        }

        add_history(query);
        char** argv = my_to_argv(query);
        argc = argvlen(argv);
        free(query);

        //write(1, utos(argvlen(argv+1)), strlen(utos(argvlen(argv+1))));
        //break;

        if (strcmp(argv[0], "cd") == 0){
            last_cmd_success = my_cd(getenv("PWD"), argv + 1);
        }else if(strcmp(argv[0], "pwd") == 0){
            last_cmd_success = pwd(getenv("PWD"));
        }else if(strcmp(argv[0], "exit")==0){
            if(argv[1] == NULL){
                exit(last_cmd_success);
            }else {
                exit(atoi(argv[1]));
            }
        }else if(strcmp(argv[0], "?") == 0){
            if(last_cmd_success!=0){
                //retourne 1 si la derniere commande exécutée etait un echec
                write(1, "1\n",2);
            }else{
                // retourne 0 sinon
                write(1,"0\n",2);
            }
        }else{
            // Exécution d'une commande externe
            last_cmd_success = execute_ext_cmd(argv);
        }

        for (int i=0; i<argc; free(argv[i++])){}
        free(argv);
    }

    return 0;
}