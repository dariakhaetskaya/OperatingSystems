#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <ulimit.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>

extern char **environ; // system variable

void showHelp();
bool checkEnvVar(char* arg);

int main(int argc, char *argv[]){
	showHelp();
	int currentArg;
	char options[] = "ispuU:cC:dvV:";
	while ((currentArg = getopt(argc, argv, options)) != EOF)
		switch (currentArg){
			case 'i': {
				printf("Real User ID is %d\n", getuid());
				printf("Effective User ID is %d\n", geteuid());
				printf("Real Group ID is %d\n", getuid());
				printf("Effective Group ID is %d\n", getegid());
				break;
			}

			case 's': {
				// pid == 0 => ID of calling proccess, pgit == 0 => "pid"-proccess is going to lead the group
				if (setpgid(0, 0) != 0){
					perror("failed to set new proccess ID.\n");
				}
				break;
			}

			case 'p':{
				printf("Proccess ID is %d\n", getpid());
				printf("Parent ID is %d\n", getppid());
				// pasing zero means it shall return the process group ID of the calling process
				printf("Group ID is %d\n", getpgid(0));
				break;
			}

			case 'u': {
				// Return the soft file size limit of the process.
				// The limit is in units of 512-byte blocks and is inherited by child processes.
				// Files of any size can be read. The return value is the integer part
				// of the soft file size limit divided by 512. 
				// If the result cannot be represented as a long int, the result is unspecified.
				printf("Soft file size limit of this process is %ld bytes\n", ulimit(UL_GETFSIZE));
				break;
			}

			case 'U': {
				// Set the hard and soft file size limits for output operations of the process 
				// to the value of the second argument, taken as a long int
				if (ulimit(UL_SETFSIZE, atol(optarg)) != atol(optarg)){
					perror("Any process may decrease its own limit, but only a process with appropriate privileges may increase the limit.\n");
				}
				break;
			}

			case 'c': {
				struct rlimit coreFileLimit;
				getrlimit(RLIMIT_CORE, &coreFileLimit);
				printf("Soft core file limit is %lu bytes\n", coreFileLimit.rlim_cur);
				printf("Hard core file limit is %lu bytes\n", coreFileLimit.rlim_max);
				break;
			}

			case 'C': { 
				struct rlimit coreFileLimit;
				coreFileLimit.rlim_cur = atol(optarg);
				coreFileLimit.rlim_max = atol(optarg);
				if (setrlimit(RLIMIT_CORE, &coreFileLimit) != 0){
					perror("Any process may decrease its own limit, but only a process with appropriate privileges may increase the limit.\n");
				}
				break;
			}

			case 'd': {
				char* buffer = get_current_dir_name();
				printf("current directory is: %s\n", buffer);
				free(buffer);
				break;
			}

			case 'v': {
				char **p;
				for (p = environ; *p; p++)
					printf ("%s\n", *p);
				break;
			}

			case 'V': {
				int i = 0;
				for (i = 0; optarg[i] == '='; optarg++);
				char* name = malloc(sizeof(char) * (i + 1));
				char* value = malloc(sizeof(char) * (strlen(optarg) - i));
				strncpy(name, optarg, i);
				strcpy(value, optarg + i + 1);

				if (setenv(name, value, 1) != 0){
					perror("Failed to set new evironment variable\n");
				}
				break;
			}

			default: {
				printf("no such option!\n");
			}
		}

	return 0;
}


void showHelp(){
  printf("-i    - real and effective user and group ID`s\n"
    "-s    - make current process leader of group\n"
    "-p    - process, parent-process and group\n"
    "-u    - get current ulimit value\n"
    "-Unew_ulimit  - set ulimit to new_ulimit\n"
    "-c    - core file size\n"
    "-Csize    - set core file size to size\n"
    "-d    - print current directory\n"
    "-v    - show environment variables\n"
    "-Vname=value  - change environment variable or create new if its not exist\n\n");
}
