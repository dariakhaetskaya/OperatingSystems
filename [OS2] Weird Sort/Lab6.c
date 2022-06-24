#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define COEF 2
#define MAXNUMSTR 100

void* printString(void *args){
    sleep (COEF * strlen((char*)args));
    printf("%s", (char*)args);
}

void freeStrings(char **strings, int strCount){
    for (int i = 0; i < strCount; i++){
        free(strings[i]);
    }
    free(strings);
}

int main() {

    char** strings = malloc(sizeof(char *) * MAXNUMSTR);

    int readCount = 2;
    int idx = 0;
    size_t strlen = 200;

    printf("Enter strings to sort:\n");

    while (readCount > 1){
        strings[idx] = malloc(sizeof(char) * strlen);
        readCount = getline(&strings[idx], &strlen, stdin);
        idx++;

        if (readCount == 1){
            free(strings[idx]);
            idx--;
        }
    }

    int strCount = idx;

    printf("Sorting your strings. . .\n");

    pthread_t* threads = malloc(sizeof(pthread_t) * strCount);

    for (int i = 0; i < strCount; i++){
        if (pthread_create(&threads[i], NULL, printString, strings[i]) != 0){
            perror("failed to create new thread");
            freeStrings(strings, strCount);
            free(threads);
            exit(-1);
        }
    }

    for (int i = 0; i < strCount; i++){
        if (pthread_join(threads[i], NULL) != 0){
            perror("failed to join thread");
            freeStrings(strings, strCount);
            free(threads);
            exit(-1);
        }
        free(strings[i]);
    }

    free(strings);
    free(threads);
    return 0;
}

