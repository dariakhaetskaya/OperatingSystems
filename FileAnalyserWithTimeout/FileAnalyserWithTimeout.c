#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define LINECOUNT 256
#define BUF_SIZE 2048

bool buildFileMap(size_t* offsets, size_t* lengths, int fileDescriptor, int* lineCount){
    char buffer[BUFSIZ];
    int offset = 0;
    int lineIdx = 1;
    int bytesRead = 1;

    while (bytesRead > 0){
        bytesRead = read(fileDescriptor, &buffer, BUFSIZ);
        if (bytesRead == -1){
            perror("failed to read\n");
            return false;
        }

        for (size_t i = 0; i < bytesRead; ++i){
            lengths[lineIdx]++;
            offset++;
            if (buffer[i] == '\n'){
                offsets[lineIdx] = offset - lengths[lineIdx];
                lineIdx++;
            }
        }
    }

    *lineCount = lineIdx;
    return true;
}

int main(int argc, char* argv[]){
    int fileDescriptor = 0;
    if (argc < 2) {
        perror("Usage: filename as argument\n");
        return 0;
    }

    if((fileDescriptor = open(argv[1], O_RDONLY)) == -1) {
        perror("Input file doesn`t exist\n");
        return 0;
    }

    size_t lengths[LINECOUNT] = {0};
    size_t offsets[LINECOUNT] = {0};

    int lineCount = 0;

    buildFileMap(offsets, lengths, fileDescriptor, &lineCount);

    fd_set descriptorSet;
    FD_ZERO(&descriptorSet);
    FD_SET(STDIN_FILENO, &descriptorSet);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Enter line number from 1 to %d. Enter 0 to exit. You only got 5 seconds\n", lineCount - 1);

    unsigned int lineToPrint = 1;
    int selectRetValue;

    while (true){
        char buffer[BUFSIZ] = {0};
        printf("line number: ");

        if (lineToPrint == 0){
            break;
        }

        selectRetValue = select(1, &descriptorSet, NULL, NULL, &timeout);

        if (selectRetValue == -1){
            perror("select(3C) failed");
            return -1;
        }

        if (selectRetValue == 0){
            lseek(fileDescriptor, 0, SEEK_SET);
            int bytesRead = 1;
            while ( bytesRead > 0 ){
                bytesRead = read(fileDescriptor, &buffer, BUFSIZ);
                if (bytesRead < BUFSIZ){
                    buffer[bytesRead] = '\0';
                }
                printf("%s", buffer);
            }
            return 0;
        }

        if (scanf("%u", &lineToPrint) != 1){
            fflush(stdin);
            perror("Invalid input");
            continue;
        }

        if (lineToPrint < 0 || lineToPrint > lineCount - 1){
            printf("Line number is an integer number from 1 to %d\n", lineCount - 1);
            continue;
        }

        if (lineToPrint == 0){
            break;
        }

        if (lseek(fileDescriptor, offsets[lineToPrint], SEEK_SET) == -1){
            perror("failed to change pointer position in file\n");
            continue;
        }

        if (read(fileDescriptor, &buffer, lengths[lineToPrint]) == lengths[lineToPrint]){
            printf("%s", buffer);
        } else {
            perror("error reading file\n");
        }

    }

    close(fileDescriptor);
    return 0;
}
