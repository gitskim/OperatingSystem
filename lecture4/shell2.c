#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void sig_int(int);

int main() {
	char buf[10000];
	pid_t pid;
	int status;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		perror("signal error");

	printf("%% ");

	while (fgets(buf, 10000, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */

		if ((pid = fork()) < 0) {
			perror("fork error");
		} else if (pid == 0) {      /* child */
			execlp(buf, buf, (char *)0);
			perror("couldn't execute");

		}

		/* parent */
		if ((pid = waitpid(pid, &status, 0)) < 0)
			perror("waitpid error");
		printf("%% ");

	}
}

void sig_int(int signo) {
	printf("interrupt\n %%");
}
