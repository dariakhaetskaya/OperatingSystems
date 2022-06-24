#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_STEPS 200000000

// thread's arguments

typedef struct ThreadWork{
    int initialIndex; // number of iteration to start with
    int iterCount; // how many iterations should thread make
    double partialSum;
} ThreadWork;

void* countPartialSum(void* args){
    ThreadWork* work = (ThreadWork *)args;
    double partialSum = 0;
    int finalIndex = work->initialIndex + work->iterCount;

    for (int i = work->initialIndex; i < finalIndex; i++){
        partialSum += 1.0 / (i * 4.0 + 1.0);
        partialSum -= 1.0 / (i * 4.0 + 3.0);
    }

    work->partialSum = partialSum * 4;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("This program calculates Pi using Leibniz series.\n"
               "Please pass it thread number to start calculation.\n");
        exit(-1);
    }

    int threadNum = atoi(argv[1]);

    if (threadNum < 1){
        printf("This program calculates Pi using Leibniz series.\n"
               "Please pass it thread number to start calculation.\n");
        exit(-1);
    }

    pthread_t* threads = calloc(threadNum, sizeof(pthread_t *));
    ThreadWork* work = calloc(threadNum, sizeof(ThreadWork));
    
    for (int i = 0; i < threadNum; i++){
        if (i == 0){
            work[i].initialIndex = 0;
            work[i].iterCount = NUM_STEPS / threadNum + NUM_STEPS % threadNum;
        } else {
            work[i].iterCount = NUM_STEPS / threadNum;
            work[i].initialIndex = work[i-1].initialIndex + work[i-1].iterCount;
        }
        
        if (pthread_create(&threads[i], NULL, countPartialSum, &work[i]) != 0 ){
            perror("failed to create new thread");
            free(threads);
            free(work);
            exit(-1);
        }
//	printf("%d\n", work[i].iterCount);
    }
    
    double pi = 0;

    for (int i = 0; i < threadNum; i++){
        if (pthread_join(threads[i], NULL) != 0 ){
            perror("failed to join thread");
            free(threads);
            free(work);
            exit(-1);
        }
        
        pi += work[i].partialSum;
    }
    
    printf("pi = %.20f\n", pi);
    free(threads);
    free(work);
    return 0;
}
