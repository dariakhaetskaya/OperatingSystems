#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define LINECOUNT 256

bool buildFileMap(off_t* offsets, off_t* lengths, int fileDescriptor, size_t* lineCount){
    char buffer[BUFSIZ];
    off_t offset = 0;
    off_t lineIdx = 1;
    off_t bytesRead = 1;

    while (bytesRead > 0){
        bytesRead = read(fileDescriptor, buffer, BUFSIZ);
        if (bytesRead == -1){
            perror("failed to read\n");
            return false;
        }

        for (off_t i = 0; i < bytesRead; ++i){
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

    off_t lengths[LINECOUNT] = {0};
    off_t offsets[LINECOUNT] = {0};

    size_t lineCount = 0;

    buildFileMap(offsets, lengths, fileDescriptor, &lineCount);

    printf("Enter line number from 1 to %d. Enter 0 to exit\n", lineCount - 1);

    size_t lineToPrint = 1;

    while (true){
        printf("line number: ");

        if (lineToPrint == 0){
            break;
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

        char buffer[BUFSIZ] = {0};

        if (read(fileDescriptor, &buffer, lengths[lineToPrint]) == lengths[lineToPrint]){
            printf("%s", &buffer);
        } else {
            perror("error reading file\n");
        }

    }
    if (close(fileDescriptor) != 0){
	perror("error occurred closing file");
    }
    return 0;
}

