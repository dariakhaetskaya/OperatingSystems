#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>

#define LEN 43

void PrintInUpper(const char* string){
    char Char;
    int i = 0;

    while (string[i] != '\0') {
        Char = string[i];
        putchar(toupper(Char));
        i++;
    }
}

int main(){
    pid_t Child_1, Child_2;
    int fileDescriptor[2];

    char OutputText[LEN] = "Greetings from the other side of the pipe\n";
    char InputText[LEN] = {0};

    if (pipe(fileDescriptor) == -1){
        perror("failed to create pipe ");
        return -1;
    }

    Child_1 = fork();

    if (Child_1 == 0){
        write(fileDescriptor[1], OutputText, LEN);
        return 0;
    }
    else if (Child_1 == -1) {
        perror("fork failed ");
        return -1;
    }

    Child_2 = fork();

    if (Child_2 == -1){
        perror("fork failed ");
        return -1;
    }

    if (Child_1 > 0 && Child_2 == 0){
        read(fileDescriptor[0], InputText, LEN);
        PrintInUpper(InputText);
        return 0;
    }

    int Status_1, Status_2;

    waitpid(Child_1, &Status_1, 1);
    waitpid(Child_2, &Status_2, 1);

    printf("Child 1 exit status: %d\n", WEXITSTATUS(Status_1));
    printf("Child 2 exit status: %d\n", WEXITSTATUS(Status_2));

    return 0;
}
