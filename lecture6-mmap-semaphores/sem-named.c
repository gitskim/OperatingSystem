#include "he.h"
#define name "msem"

int main() {
	pid_t pid = fork();
	static int val = 7;
	if (pid == 0) {
		// child
		sem_t *sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 1);
		sem_wait(sem);
		printf("child waited\n");
		val++;
		printf("child value %d\n", val); // the value should be 8.
		sem_post(sem);
	} else if (pid > 0) {
		// parent
		sleep(1);
		sem_t *sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 1);
		sem_wait(sem);
		printf("parent waited\n");
		val++;
		printf("parent value %d\n", val); // the value should be 8
		sem_post(sem);
	}
}

// NOTE: val can't be shared between child and parent.
