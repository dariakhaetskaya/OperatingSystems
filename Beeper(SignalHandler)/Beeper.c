#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static void sig_usr(int);
int pipiPupuCount = 0;

int main (){
    if (sigset(SIGINT, sig_usr) == SIG_ERR){
        perror("failed to handle SIGINT");
        exit(1);
    }

    if (sigset(SIGQUIT, sig_usr) == SIG_ERR){
        perror("failed to handle SIGQUIT");
        exit(1);
    }
    for ( ; ; )
        pause();
}

static void sig_usr(int signo) {
    char buffer[50];

    if (signo == SIGINT){
        size_t n = sprintf(buffer, "\a\n");
        write(STDIN_FILENO, buffer, n);
        pipiPupuCount++;
    }
    else if (signo == SIGQUIT){
        size_t n = sprintf(buffer,"\nbeeped %d times\n", pipiPupuCount);
        write(STDIN_FILENO, buffer, n);
	_exit(0);
    }
    else {
        size_t n = printf(buffer, "\ngot signal %d\n", signo);
        write(STDIN_FILENO, buffer, n);
    }
}

