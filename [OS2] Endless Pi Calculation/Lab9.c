#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#define ITERATION_STEP 2000

bool exitFlag = false;
pthread_mutex_t mutex;
int threadNum = 1;
pthread_barrier_t barrier;

int maxIterationReached;

// thread's arguments

typedef struct ThreadWork{
    int initialIndex; // number of iteration to start with
    double partialSum;
} ThreadWork;

void* countPartialSum(void* args){
    ThreadWork* work = (ThreadWork *)args;
    double partialSum = 0;
    int stepNumber = work->initialIndex;
    long int iterationCount = 0;

    for (int i = stepNumber; ; i += threadNum) {
        partialSum += 1.0 / (i * 4.0 + 1.0);
        partialSum -= 1.0 / (i * 4.0 + 3.0);
        iterationCount++;

        if (iterationCount % ITERATION_STEP == 0){
            pthread_barrier_wait(&barrier);
            if (exitFlag || iterationCount < maxIterationReached){
                printf("on thread %lu exit flag is true. Unlocking mutex and terminating. ITER COUNT = %d\n", pthread_self(), iterationCount);
                work->partialSum = partialSum * 4;
                pthread_exit(NULL);
            } else {
                stepNumber += threadNum * ITERATION_STEP;
                pthread_mutex_lock(&mutex);
                if (iterationCount > maxIterationReached){
                    maxIterationReached = iterationCount;
                }
                pthread_mutex_unlock(&mutex);
            }
        }
    }

}

void signalHandler(int signal){
    exitFlag = true;
}

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("This program calculates Pi using Leibniz series.\n"
               "Please pass it thread number to start calculation.\n");
        exit(-1);
    }
    sigset_t maskWithBlockedSIGINT;
    sigset_t defaultMask;

    sigaddset(&maskWithBlockedSIGINT, SIGINT);

    signal(SIGINT, signalHandler);
    pthread_sigmask(SIG_SETMASK, &maskWithBlockedSIGINT, &defaultMask);
    pthread_mutex_init(&mutex, NULL);


    threadNum = atoi(argv[1]);

    if (threadNum < 1){
        printf("This program calculates Pi using Leibniz series.\n"
               "Please pass it thread number to start calculation.\n");
        exit(-1);
    }

    if (pthread_barrier_init(&barrier, NULL, threadNum) != 0) {
        perror("failed to init barrier");
        exit(-1);
    }

    pthread_t* threads = calloc(threadNum, sizeof(pthread_t *));
    ThreadWork* work = calloc(threadNum, sizeof(ThreadWork));
    time_t start,end;
    time (&start);

    for (int i = 0; i < threadNum; i++){

        work[i].initialIndex = i;

        if (pthread_create(&threads[i], NULL, countPartialSum, &work[i]) != 0 ){
            perror("failed to create new thread");
            free(threads);
            free(work);
            exit(-1);
        }
    }

    pthread_sigmask(SIG_SETMASK, &defaultMask, &maskWithBlockedSIGINT);

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
    time (&end);
    double dif = difftime (end,start);
    printf("pi = %.14f\n", pi);
    printf ("Your calculations took %.10lf seconds to run.\n", dif );

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    free(threads);
    free(work);
    return 0;
}
