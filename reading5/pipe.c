#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#define MAXLINE 1000
int main() {
	int n;
	int fd[2];
	pid_t pid;
	char line[MAXLINE];
	//Two file descriptors are returned through the fd argument: fd[0] is open for reading, and fd[1] is open for writing. The output of fd[1] is the input for fd[0].
	if (pipe(fd) < 0) {
		perror("pipe error");
	}
	if ((pid = fork()) < 0) {
		perror("fork error");
	} else if (pid > 0) {
	// parent
		close(fd[0]);
		write(fd[1], "hello world\n", 12);
	} else {
	// child
		close(fd[1]);
		n = read(fd[0], line, MAXLINE); //question? what if you read from 1? read(fd[1])? would this be unpredictable?
		write(STDOUT_FILENO, line, n);
	}
}
