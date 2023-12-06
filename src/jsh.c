#include "utils.h"

char* mkprompt(job_list* jobs, char* cur_path){

    // Allocation de l'espace stockant le prompt
    char* prompt = calloc(PATH_MAX, sizeof(char));

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

    int last_cmd_success = 0;
    int argc;

    // Boucle lisant l'entrée utilisateur.
   for (;;){
        char* prompt = mkprompt(jobs, getenv("PWD"));
        char* query = readline(prompt);

        free(prompt);

        if (!query){
            // Ctrl+D ou problème d'allocation dans readline
            exit(last_cmd_success);
        }

        add_history(query);

	    int empty_line = 1;
	    for(int i=0; i< strlen(query); i++){
            if (query[i] != ' '){empty_line = 0;}
        }

        if (empty_line){
            free(query);
            continue;
        }
	    char** argv = my_to_argv(query);
        argc = argvlen(argv);
        free(query);

        // Récupération de la valeur des
        // variables d'environnement
        char* is_env = calloc(argc, sizeof(char));
        char* k;
        for (int i = 0; i<argc; i++){
            if (argv[i][0] == '$'){
                k = getenv(argv[i]+1);
                if (k){
                    argv[i] = k;
                    is_env[i]++;
                }
            }
        }

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
            char* lcs = utos(last_cmd_success);
            if (write(STDOUT_FILENO, lcs, strlen(lcs)) > 0){
                write(STDOUT_FILENO, "\n", 1);
                last_cmd_success = 0;
            } else {
                last_cmd_success = 1;
            }
            free(lcs);
        }else if(strcmp(argv[0],"jobs")==0){ //jobs sans argument
            affiche_jobs(jobs);

        }else{
            // Exécution d'une commande externe
            job_node* newJob=new_job_node(argv);
            add_job_to_list(jobs,newJob);
            last_cmd_success = execute_ext_cmd(argv);
        }

        for (int i=0; i<argc; i++){
            if (!is_env[i]){free(argv[i]);}
        }
        free(argv);
        free(is_env);
    }

    
    return 0;
}
