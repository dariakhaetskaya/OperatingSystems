#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define LINECOUNT 256

void buildFileMap(off_t* offsets, off_t* lengths, const char* mappedFile, int fileSize, size_t* lineCount){
    char buffer[BUFSIZ];
    off_t offset = 0;
    off_t lineIdx = 1;

    for (off_t i = 0; i < fileSize; ++i){
        lengths[lineIdx]++;
        offset++;
        if (*(mappedFile + i) == '\n'){
            offsets[lineIdx] = offset - lengths[lineIdx];
            lineIdx++;
        }
    }

    *lineCount = lineIdx;
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

    size_t fileSize = lseek(fileDescriptor, 0, SEEK_END);

    char* mappedFile = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);

    off_t lengths[LINECOUNT] = {0};
    off_t offsets[LINECOUNT] = {0};

    size_t lineCount = 0;

    buildFileMap(offsets, lengths, mappedFile, fileSize,  &lineCount);

    fd_set descriptorSet;
    FD_ZERO(&descriptorSet);
    FD_SET(STDIN_FILENO, &descriptorSet);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Enter line number from 1 to %d. Enter 0 to exit. You only got 5 seconds\n", lineCount - 1);

    size_t lineToPrint = 1;
    int selectRetValue;

    while (true){
        char buffer[BUFSIZ] = {0};

        if (lineToPrint == 0){
            break;
        }

        selectRetValue = select(1, &descriptorSet, NULL, NULL, &timeout);


        if (selectRetValue == -1){
            perror("select(3C) failed");
            return -1;
        }

        if (selectRetValue == 0){
            for (off_t i = 0; i < fileSize; ++i){
                printf("%c", *(mappedFile + i));
            }
            printf("\n");
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

        for (off_t i = offsets[lineToPrint]; i < offsets[lineToPrint+1]; ++i){
            printf("%c", *(mappedFile + i));
        }

    }
    munmap(mappedFile, fileSize);
    close(fileDescriptor);
    return 0;
}


