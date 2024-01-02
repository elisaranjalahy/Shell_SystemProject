#include "utils.h"


char** parse_pipes(char** argv){
    int i = 0; int pipe_fd[2];
    while (argv[i] && argv[i++][0] != '|');
    if (pipe(pipe_fd) < 0) return NULL;

    if (argv[i]){
        if (fork()) {
            close(pipe_fd[1]);
            dup2(pipe_fd[0], 0);
            return parse_pipes(argv + i);
        } else {
            close(pipe_fd[0]);
            dup2(pipe_fd[1], 1);
            for (int j = i-1; argv[j]; free(argv[j++]));
            argv[i-1] = NULL;
        }
    }
    return argv;
}

int mkrdr(int new_fd, const char* filename, int flags, mode_t mode){
    int fd;
    if ((fd = open(filename, flags, mode)) < 0){
        perror("open");
        return 1;
    }
    if (dup2(fd, new_fd) != new_fd){
        perror("dup2");
        return 1;
    }
    close(fd);
    return 0;
}

int mkflags(char* rdr){
    if (rdr[0] == '2'){rdr++;}
    if (rdr[0] == '>'){
        int flags = O_WRONLY | O_CREAT;
        if (rdr[1] == '\0'){return flags | O_EXCL;}
        else if (rdr[1] == '>' && rdr[2] == '\0'){return flags | O_APPEND;}
        else if (rdr[1] == '|' && rdr[2] == '\0'){return flags | O_TRUNC;}
    }
    return 0;
}

int redirections(char** argv){
    int incr = 1;
    for (int i = 0; i < argvlen(argv) - 1; i += incr){
        incr = 1;

        if (argv[i][0] == '>'){
            int flags = mkflags(argv[i]);
            if (!flags){continue;}

            if (mkrdr(STDOUT_FILENO, argv[i+1], flags, 420)){return 1;}
            incr = 0;
        } else if (argv[i][0] == '2'){
            int flags = mkflags(argv[i]);
            if (!flags){continue;}

            if (mkrdr(2, argv[i+1], flags, 420)){return 1;}
            incr = 0;
        } else if (argv[i][0] == '<' && argv[i][1] == '\0'){
            int fd;
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                perror("open");
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) != STDIN_FILENO){
                perror("dup2");
                return 1;
            }
            incr = 0;

        }

        if (incr == 0){
            int len = argvlen(argv);
            free(argv[i]); free(argv[i+1]);
            for(int k = i; k < len - 2; k++){
                argv[k] = argv[k+2];
            }
            argv[len-1] = NULL; argv[len-2] = NULL;
        }
    }
    return 0;
}
