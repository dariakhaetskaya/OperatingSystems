#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define LEN 43

int main(){
    char Char;
    char string[LEN];
    int readCount;

    while ((readCount = read(STDIN_FILENO, string, LEN)) > 0){
        for (int i = 0; i < readCount; i++){
            Char = string[i];
            putchar(toupper(Char));
        }
    }

    return 0;
}
