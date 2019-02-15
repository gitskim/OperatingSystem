#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main() {
	char buf[4000];
	pid_t pid;
	int status;

	printf("%% "); //print f requires %% to print %

	// stdio fgets to read one line at a time from the stdinput. When we type ctrl-D, which is the end-of-file character, fgets returns a null pointer and the loop stops and the process terminates. 
	while (fgets(buf, 4000, stdin) != NULL) {
		// READ from stdin
		if (buf[strlen(buf) - 1] == '\n') {
			buf[strlen(buf) - 1] = 0; // replace newline with null
		}

		// Open a new process with fork
		// fork returns twice: once in the parent and once in the child. 
		if ((pid = fork()) < 0 ) {
			perror("fork error");
		} else if (pid == 0) {
			// child process
			execlp(buf, buf, (char *)0);
		} else {
			// parent process
			if ((pid = waitpid(pid, &status, 0)) < 0) {
				perror("waitpid error");
			}
			printf("%% ");
		}

	}
}
