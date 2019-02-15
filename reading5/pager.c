#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#define MAXLINE 1000
int main(int argc, char **argv) {
	int n;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[MAXLINE];
	FILE *fp;

    //NOTE: pager is reading from stdin and nicely formats what it read. pagers are a text reader.
	if (argc != 2) {
		perror("wrong usage");
	}

	if ((fp = fopen(argv[1], "r")) == NULL)
		perror("can't open");

	if (pipe(fd) < 0)  // those file descriptors are now a pipe now.
		perror("pipe error");

	if ((pid = fork()) < 0) 
		perror("fork error");
	else if(pid >0) { // parent - reads from a file and writes it to the pipe
		close(fd[0]); // close input reading

		while (fgets(line, MAXLINE, fp) != NULL) { // parent reads from the file and writes to the pipe
			n = strlen(line);
			if (write(fd[1], line, n) != n) perror("write error");
		}

		if (ferror(fp))
			perror("fgets error");

		close(fd[1]); // close pipe after it's done

		if (waitpid(pid, NULL, 0) < 0)
			perror("waitpid failed");
	} else {
		// child - reads from the pipe, redirects stdin of the pager, so that it comes from the pipe and execute a pager
		close(fd[1]); //close write output

		// this null check is unnecessary, but it's done as a safety check.
		if (fd[0] != STDIN_FILENO) {
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) { // STDIN_FILENO is redirected to fd[0]. stdin is now fd[0]
			    // dup2 first closes STDIN_FILENO and make it available. Then it makes STDIN_FILENO be the same as fd[0]
				perror("dup2 error to stdin");
			}

			fprintf(stderr, "stdin fileno: %d\n", STDIN_FILENO);
			// you don't need the fd[0] anymore, because whatever the address fd[0] was pointing to is now being pointed by stdin_fileno
			close(fd[0]);
		}

		if ((pager = getenv("PAGER")) == NULL)
			pager = "./hello"; // default pager value
		
		//what is the purpose of this if check? -> to remove the '/'
		if ((argv0 = strrchr(pager, '/')) != NULL) //strrchr = returns a pointer to the last occurrence of a character
			argv0++;
		else 
			argv0 = pager;
		//pager and argv0 are the same thing e.g. execl("/home/vlc", "/home/vlc", "movie.mkv", NULL);
		// ?question? how come file descriptor table is not wiped out with execl?
		if (execl(pager, argv0, (char *)0) < 0) //
			perror("execl error");
	}

}
