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
    pid_t newPid, ret;

    if ((newPid = fork()) == -1) {
        perror("fork failed: ");
        return 1;
    }

    if (newPid == 0){
        printf("I'm a child process with pid: %d\n", newPid);
        if ( execl("/bin/cat", "cat", argv[1], NULL) == -1){
            perror("execl failed: ");
            return 1;
        }
    }

    printf("I'm a parent process waiting for my child to die: %d\n", newPid);
    ret = wait(&status);

    if (ret == -1){
        perror("wait(2) error: ");
        return 1;
    }

    printf("My child died. Now it's time for me to pass away\n");

    return 0;
}
