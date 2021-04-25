#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#define COMMAND_LEN 16

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("pass this program a file to count empty lines in it\n");
    }

    char command[COMMAND_LEN + strlen(argv[1]) + 1];

    if (sprintf(command, "grep ^$ %s | wc -l", argv[1]) < 0){
        printf("error occured\n");
        return -1;
    }

    FILE* outputStream = popen(command, "r");

    if(outputStream == NULL){
        perror("failed to create pipe");
        return -1;
    }

    int emptyLineCount;
    fscanf(outputStream, "%d", &emptyLineCount);
    pclose(outputStream);
    printf("Number of empty lines: %d\n", emptyLineCount);

    return 0;
}
