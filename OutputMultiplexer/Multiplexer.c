#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include <ctype.h>
#include <signal.h>

#define TIME_OUT 1

int alarmBeeped = 0;
FILE** fd;

void alarmHandler(int signal);

rlim_t getMaxOpen(){
    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    return limit.rlim_cur;
}

int openFiles(int fileCount, char** files){
    int openFilesCount = 0;
    for (int i = 1; i < fileCount; i++){
        files[openFilesCount] = files[openFilesCount + 1];
        printf("opening %s\n", files[openFilesCount]);

        if ((fd[openFilesCount] = fopen(files[openFilesCount], "rb")) == NULL){
            fprintf(stderr, "%s: cannot open file\n", files[openFilesCount]);
            continue;
        } else {
            openFilesCount++;
        }
    }
    return openFilesCount;
}

void alarmHandler(int sig){
    alarmBeeped = 1;
}


void readFiles(int fileCount, char* argv[]){
    sigset(SIGALRM, alarmHandler);
    siginterrupt(SIGALRM, 1);
    int openedFiles = fileCount;
    char buffer[BUFSIZ];
    char tmpBuffer[BUFSIZ];

    while (openedFiles){
        for(int i = 0; i < fileCount; i++){
            if (fd[i] == NULL){
                continue;
            }

            alarm(TIME_OUT);

            if (fgets(buffer, BUFSIZ, fd[i]) == NULL && !alarmBeeped){
                fclose(fd[i]);
                fd[i] = NULL;
                openedFiles--;
            } else if (alarmBeeped) {
                alarmBeeped = 0;
                continue;
            } else {
                sprintf(tmpBuffer, "%s: %s", argv[i], buffer);
                write(1, tmpBuffer, strlen(tmpBuffer));
                memset(buffer, 0, BUFSIZ);
                memset(tmpBuffer, 0, BUFSIZ);
            }
            alarm(0);
        }
    }
    sigset(SIGALRM, SIG_DFL);
}


int main(int argc, char* argv[]){
    if (argc < 2){
        printf("pass this program multiple files to start multiplexed printing\n");
        return 0;
    } else if (argc > getMaxOpen()){
        printf("too many files\n");
        return 0;
    }

    fd = (FILE**)malloc(argc*sizeof(FILE));
    if (fd == NULL){
        perror("malloc(3C) failed to allocate memory\n");
        return -1;
    }
    int opened = openFiles(argc, argv);

    if (opened < argc - 1){
        printf("opened %d files, do you want to continue? Y/N\n", opened);
        char answer;
        scanf("%c", &answer);
        answer = tolower(answer);
        if (answer == 'y'){
            readFiles(opened, argv);
        } else {
            for (int i = 0; i < opened; i++){
                fclose(fd[i]);
            }
            return 0;
        }
    }

    readFiles(opened, argv);

    return 0;
}
