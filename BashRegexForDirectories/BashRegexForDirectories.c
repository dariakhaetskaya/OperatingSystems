#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <string.h>
#include <glob.h>

int main(int argc, char **argv) {

    char* default_args[] = { "empty pathname", NULL };
    char** pattern = (1 < argc) ? argv : default_args;

    glob_t result;
    int errCheck = glob(pattern[1], GLOB_NOCHECK, NULL, &result);
    if (errCheck == GLOB_NOSPACE){
        perror("running out of memory\n");
        return 0;
    }
    if (errCheck == GLOB_ABORTED){
        perror("read error\n");
        return 0;
    }
	if (errCheck == GLOB_NOSYS){
        perror("the requested function is not supported\n");
        return 0;
    }

    for (int i = 0; i < result.gl_pathc; i++){
        printf("%s\n", result.gl_pathv[i]);
    }

    globfree(&result);
    return 0;
}
