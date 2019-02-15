#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv) {
	DIR *dp;
	struct dirent *dirp;

	if (argc != 2) {
		perror("usage: ls directory_name");
	}

	if ((dp = opendir(argv[1])) == NULL) {
		perror("can't open");
	}

	while ((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);

	closedir(dp);
	return 0;
}
