#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

static void sig_usr(int);
int pipiPupuCount = 0;
bool turnOffBeeper = false;

int main (){
    if (sigset(SIGINT, sig_usr) == SIG_ERR){
        perror("failed to handle SIGINT");
        exit(1);
    }

    if (sigset(SIGQUIT, sig_usr) == SIG_ERR){
        perror("failed to handle SIGQUIT");
        exit(1);
    }
    for ( ; ; ){
        pause();
        if (turnOffBeeper){
            printf("\nbeeped %d times\n", pipiPupuCount);
            exit(0);
        }
    }
}

static void sig_usr(int signo) {
    if (signo == SIGINT){
        write(STDIN_FILENO, "\a\n", 2);
        pipiPupuCount++;
    }
    else if (signo == SIGQUIT){
        turnOffBeeper = true;
    }
}
