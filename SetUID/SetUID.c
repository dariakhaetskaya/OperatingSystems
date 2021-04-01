#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void printID(){
	printf("Real user ID: %d\n", getuid());
	printf("Effective user ID: %d\n", geteuid());
}

void tryOpening(char *fileName){
	FILE *file;
	file = fopen(fileName, "r");

	if (file == NULL){
		perror("can't open file");
	} else {
		printf("file openned successfully\n");
		if (fclose(file) == EOF){
			perror("can't close file");
		}
	}
}

int main(int argc, char *argv[]){
	printID();
	tryOpening(argv[1]);

	int setNewUID = seteuid(getuid());

	if (setNewUID == -1){
		perror("failed to set new user ID\n");
	}
	
	printID();
	tryOpening(argv[1]);
	return 0;
}
