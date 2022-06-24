#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#define NUM_STRINGS 10

int main() {

    sem_t* semParent = sem_open("/ParentSem", O_CREAT, 0777, 0);
    sem_t* semChild = sem_open("/ChildSem", O_CREAT, 0777, 1);

    switch (fork()) {
        case -1:
            perror("Fork failed");
            sem_destroy(semParent);
            sem_destroy(semChild);
            exit(-1);
        case 0:
            for (int i = 1; i <= NUM_STRINGS; i++) {
                sem_wait(semChild);
                printf("PARENT: %d\n", i);
                sem_post(semParent);
            }
            break;
        default:
            for (int i = 1; i <= NUM_STRINGS; i++) {
                sem_wait(semParent);
                printf("CHILD: %d\n", i);
                sem_post(semChild);
            }
            break;
    }
    sem_close(semParent);
    sem_close(semChild);
    return 0;

}
