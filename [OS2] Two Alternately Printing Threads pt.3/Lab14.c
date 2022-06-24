#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>

sem_t sem1;
sem_t sem2;

void* threadRoutine(void* args){
    for(int i = 0; i < 10; i++){
        sem_wait(&sem2);
        printf("pupu\n");
        sem_post(&sem1);
    }
}

int main(){

    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);

    pthread_t childThread;

    if (pthread_create(&childThread, NULL, threadRoutine, NULL)){
        perror("pthread_create:");
    }

    for(int i = 0; i < 10; i++){
        sem_wait(&sem1);
        printf("pipi\n");
        sem_post(&sem2);
    }

    sem_destroy(&sem1);
    sem_destroy(&sem2);

    pthread_exit(0);
}
