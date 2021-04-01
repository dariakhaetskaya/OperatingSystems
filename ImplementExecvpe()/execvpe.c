#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

extern char** environ;

int execvpe(char* file, char* argv[], char* envp[]) {
    if(file == NULL) {
        perror("NULL pointer to file");
        return -1;
    }
    char** savedEnviron = environ;
//    environ = envp;
    execvp(file, argv);
    environ = savedEnviron;
    return -1;
}

int main(int argc, char* argv[], char* envp[]) {
    if(argc < 2) {
        printf("Not enough arguments");
        return 0;
    }

    if(envp == NULL) {
        perror("Environment is not set");
    }

    if(putenv("PATH=~/lab11/dir/:/bin/:.") == -1) {
        perror("'putenv' error");
    }

    execvpe(argv[1], &argv[1], envp);
    perror(argv[1]);
    return 0;
}
