#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

pthread_mutex_t mutex;
pthread_cond_t conditionalVariable;

bool parentPrinted = false;

void* threadRoutine(void* args){
    for(int i = 0; i < 10; i++){
        pthread_mutex_lock(&mutex);
        while (!parentPrinted){
            pthread_cond_wait(&conditionalVariable, &mutex);
        }
        printf("child\n");
        parentPrinted = false;
        pthread_cond_signal(&conditionalVariable);
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&conditionalVariable, NULL);

    pthread_t childThread;

    if (pthread_create(&childThread, NULL, threadRoutine, NULL)){
        perror("pthread_create:");
    }

    for(int i = 0; i < 10; i++){
        pthread_mutex_lock(&mutex);
        while (parentPrinted){
            pthread_cond_wait(&conditionalVariable, &mutex);
        }
        printf("parent\n");
        parentPrinted = true;
        pthread_cond_signal(&conditionalVariable);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);

    pthread_exit(0);
}
