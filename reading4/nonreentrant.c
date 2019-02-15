#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

static void my_alarm(int signo) {
	struct passwd *rootptr;
	printf("in signal handler\n");
	if ((rootptr = getpwnam("root")) == NULL)
		perror("getpawnam error");
}
int main() {
	struct passwd *ptr;
	signal(SIGALRM, my_alarm);
	alarm(1);

	for (;;) {
		if ((ptr = getpwnam("sar")) == NULL)
			perror("getpwnam error");
		if (strcmp(ptr->pw_name, "sar") != 0)
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
	}
}
