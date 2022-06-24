#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>

int main() {
	srand(time(NULL));
	FILE * fd[2];
	if(p2open("sort -n", fd) == -1){
		perror("p2open failed");
		return -1;
	} 

	int currentNum;

	printf("Generated array:\n");
	for(int i = 0; i < 10; i ++){
		for(int j = 0; j < 10; j++){
			currentNum = rand() % 100;
			fprintf(fd[0], "%d\n", currentNum);
			printf("%3d ", currentNum); 
		}
		printf("\n");
	}
	if (fclose(fd[0]) != 0){
		perror("pclose failed");
		return -1;
	}

	printf("\n");

	printf("Sorted array:\n");
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++){
			fscanf(fd[1], "%d", &currentNum);
			printf("%3d ", currentNum);
		}
		printf("\n");
	}

	if (pclose(fd[1]) == -1){
		perror("pclose failed");
		return -1;
	}
	return 0;
}
