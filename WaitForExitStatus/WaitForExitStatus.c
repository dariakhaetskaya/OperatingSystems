#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main (int argc, char **argv){
    if (argc < 2){
        printf("not enough arguments\n");
        return 0;
    }

    int status;
    pid_t pid, ret;

    if ((pid = fork()) == -1){
        perror("fork failed: ");
        return 1;
    }

    if (pid == 0){
        if (execvp(argv[1], &argv[1]) == -1){
            perror("failed to execute: ");
            return 1;
        }
    }

    ret = wait(&status);

    if (ret == -1){
        perror("wait(2) error: ");
        return 1;
    }

    if (WIFEXITED(status) != 0) {
        int exit_status = WEXITSTATUS(status);
        printf("Exit status: %d\n", exit_status);
    }

    if (WIFSIGNALED(status) != 0) {
        int sig_num = WTERMSIG(status);
        printf("Signal %d caused ending program\n", sig_num);
    }

    return 0;
}
