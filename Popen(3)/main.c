#include <stdio.h>

#define LEN 43

int main(){

    char OutputText[LEN] = "Greetings from the other side of the pipe\n";

    FILE *pipe = popen("./printInUpper", "w");
    if (pipe == NULL){
        perror("failed to create pipe");
        return -1;
    }

    fwrite(OutputText, sizeof(char), LEN, pipe);
    pclose(pipe);
    return 0;
}
