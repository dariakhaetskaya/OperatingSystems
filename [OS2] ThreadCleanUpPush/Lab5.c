#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define EXECUTE 1

void onExit(void *args){
    printf("terminating thread #%lu\n", pthread_self());
}

void* printString(void *args){
    pthread_cleanup_push(onExit, NULL);
    while (true){
        printf("botay!\n");
    }
    pthread_cleanup_pop(EXECUTE);
    pthread_exit(0);
}

int main() {

    pthread_t endlessThread;

    if (pthread_create(&endlessThread, NULL, printString, NULL) != 0){
        perror("failed to create new thread");
        exit(-1);
    }

    sleep(2);
    printf("\n");

    if (pthread_cancel(endlessThread) != 0){
        perror("failed to cancel execution of new thread");
        exit(-1);
    }

    if (pthread_join(endlessThread, NULL) != 0){
        perror("failed to join new thread");
        exit(-1);
    }
    return 0;
}

