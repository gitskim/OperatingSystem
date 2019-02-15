#include <signal.h>
#include <unistd.h>
#include <stdio.h>

static void sig_alarm(int signo) {
	// do nothing
}

unsigned int sleep(unsigned int seconds) {
	if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
		return(seconds);
	}
	alarm(seconds);
	pause();
	return(alarm(0));
}

int main() {
	sleep(2);
}
