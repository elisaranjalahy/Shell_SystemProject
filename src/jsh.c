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

int parse(int argc, char** argv, int bg, int lcss, job_list* jobs){
    int last_cmd_success = lcss;

    //cd
    if (strcmp(argv[0], "cd") == 0){
        last_cmd_success = my_cd(getenv("PWD"), argv + 1);

    //kill
    } else if (strcmp(argv[0], "kill") == 0){
        last_cmd_success = my_kill(argc, argv, jobs);

    //pwd
    // Cette commande est traitée à part car contrairement
    // aux autres elle n'a pas d'intéraction avec le shell

    //exit
    } else if (strcmp(argv[0], "exit") == 0){
        if(exit_possible(jobs)){
            if(argv[1] == NULL){
                exit(last_cmd_success);
            }else {
                exit(atoi(argv[1]));
            }
        }else {
            perror("Job encore en cours d'exécution ou suspendus");
            last_cmd_success=1;
        }

    //?
    } else if (strcmp(argv[0], "?") == 0){
        char* lcs = utos(last_cmd_success);
        if (write(STDOUT_FILENO, lcs, strlen(lcs)) > 0){
            write(STDOUT_FILENO, "\n", 1);
            last_cmd_success = 0;
        } else {
            last_cmd_success = 1;
        }
        free(lcs);

    //jobs
    } else if (strcmp(argv[0],"jobs") == 0){ //jobs sans argument
        if (argc == 1) affiche_jobs(jobs, 0);
        else if (!strcmp(argv[1], "-d")) affiche_jobs(jobs, 1);

    //fg
    }else if (strcmp(argv[0],"fg")==0){
        last_cmd_success = foreground(argv, jobs);

    }else if (strcmp(argv[0],"bg")==0){
        last_cmd_success = background(argv, jobs);

    //Exécution d'une commande externe
    } else {
        last_cmd_success = execute_ext_cmd(argv, jobs, 1-bg);
    }
    return last_cmd_success;
}



int main(){

    job_list* jobs = new_job_list();

    rl_outstream = stderr;  // Affichage du prompt sur la sortie erreur

    int last_cmd_success = 0;
    int argc; int is_background;

    setup_signals(SIG_IGN);

    int stdin_fd = dup(0);
    int stdout_fd = dup(1);
    int stderr_fd = dup(2);

    // Boucle lisant l'entrée utilisateur.
    for (;;){

        purge_job_list(jobs);
        char* prompt = mkprompt(jobs, getenv("PWD"));
        char* query = readline(prompt);
        is_background = 0;

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
        free(query);

        argc = argvlen(argv);

        if (parse_erreur_syntaxe(argc, argv)){
            for (int i=0; i < argc; free(argv[i++]));
            free(argv);
            continue;
        }

        if (strcmp(argv[argc-1], "&") == 0){
            free(argv[argc-1]);
            argv[argc-1] = NULL;
            argc--;
            is_background++;
        }

        last_cmd_success = parse(argc, argv, is_background, last_cmd_success, jobs);


        for (int i = 0; i < argc; i++){
            if (argv[i]) free(argv[i]);
        }
        free(argv);

        dup2(stdin_fd, 0);
        dup2(stdout_fd, 1);
        dup2(stderr_fd, 2);
    }
    return 0;
}
