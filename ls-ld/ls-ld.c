#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>

int main(int argc, char *argv[]){
	struct stat buf;

	char mode[11];

	for (int i = 1; i < argc; i++){
		printf("\nfile: %s\n", argv[i]);
		if (lstat(argv[i], &buf) < 0){
			perror("lstat syscall error");
			continue;
		}

		if (S_ISREG(buf.st_mode)){
			mode[0] = '-';
		} else if (S_ISDIR(buf.st_mode)){
			mode[0] = 'd';
		} else {
			mode[0] = '?';
		}

		// user
		mode[1] = (buf.st_mode & S_IRUSR) ? 'r' : '-';
		mode[2] = (buf.st_mode & S_IWUSR) ? 'w' : '-';
		mode[3] = (buf.st_mode & S_IXUSR) ? 'x' : '-';

		// group
		mode[4] = (buf.st_mode & S_IRGRP) ? 'r' : '-';
		mode[5] = (buf.st_mode & S_IWGRP) ? 'w' : '-';
		mode[6] = (buf.st_mode & S_IXGRP) ? 'x' : '-';

		// others
		mode[7] = (buf.st_mode & S_IROTH) ? 'r' : '-';
		mode[8] = (buf.st_mode & S_IWOTH) ? 'w' : '-';
		mode[9] = (buf.st_mode & S_IXOTH) ? 'x' : '-';

		mode[10] = '\0';

		printf("%s ", mode);

		printf("\t%lu", buf.st_nlink);

		struct passwd *passwdPtr = getpwuid(buf.st_uid);
		if (passwdPtr != NULL){
			printf("\t%u", passwdPtr->pw_uid);
		} else {
			printf("\t%d", buf.st_uid);
		}

		struct group *groupPtr = getgrgid(buf.st_gid);
		if (groupPtr != NULL){
			printf("\t%s", groupPtr->gr_name);
		} else {
			printf("\t%d", buf.st_gid);
		}

		if (mode[0] == '-'){
			printf("\t%lu", buf.st_size);
		}

		char lastChange[FILENAME_MAX] = {0};
		strftime(lastChange, FILENAME_MAX, "%H:%M %e %b %Y", localtime(&(buf.st_ctime)));
		printf("\t%s", lastChange);

		printf("\t%s\n", basename(argv[i]));
	}

	return 0;
}
