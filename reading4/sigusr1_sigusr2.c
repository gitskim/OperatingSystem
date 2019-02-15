//User defined signals means that these signals have no definite meaning unlike SIGSEGV, SIGCONT,  SIGSTOP, etc. The kernel will never send SIGUSR1 or SIGUSR2 to a process, so the meaning of the signal can be set by you as per the needs of your application. 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static void sig_user(int); //a handler function for both signals

int main() {
	if (signal(SIGUSR1, sig_user) == SIG_ERR)
		perror("can't catch SIGUSR1");
	if (signal(SIGUSR2, sig_user) == SIG_ERR)
		perror("can't catch SIGUSR2");
	for (;;)
		pause();
}

static void sig_user(int signo) {
	if (signo == SIGUSR1) {
		printf("received SIGUSR1\n");
	} else if (signo == SIGUSR2)
		printf("received SIGSUR2\n");
	else 
		perror("bad signal");
}

