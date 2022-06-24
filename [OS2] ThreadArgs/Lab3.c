#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *printString(void *args) {
    for (char **s = (char **)args; *s != NULL; ++s) {
        printf("%s\n", *s);
    }
    return((void *)0);
}

int main() {
   pthread_t threads[4];

   int status = 0;

   char *args[][4] = {
            {"1 thread:: 1 line", "1 thread:: 2 line", "1 thread:: 3 line", NULL},
            {"2 thread:: 1 line", "2 thread:: 2 line", "2 thread:: 3 line", NULL},
            {"3 thread:: 1 line", "3 thread:: 2 line", "3 thread:: 3 line", NULL},
            {"4 thread:: 1 line", "4 thread:: 2 line", "4 thread:: 3 line", NULL}
    };


   for (int i = 0; i < 4; i++){
       status = pthread_create(&threads[i], NULL, printString, (void*)args[i]);

       if (status != 0){
           perror("failed to create thread");
           exit(-1);
       }
   }

    for (int i = 0; i < 4; i++){
        status = pthread_join(threads[i], NULL);

        if (status != 0){
            perror("failed to join thread");
            exit(-1);
        }
    }

    return 0;
}
