#include "he.h"
#include <fcntl.h>
#include <sys/mman.h>

#define NLOOPS 1000
#define SIZE sizeof(long)

static int update(long *ptr) {
	return ((*ptr)++);
}

int main() {
	int fd, i, counter;
	pid_t pid;
	void *area;

	if ((fd = open("/dev/zero", O_RDWR)) <0) perror("open dev error");

	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) perror("mmap error");

	close(fd);

	if ((pid = fork()) < 0) {
		perror("fork failed");
	} else if (pid > 0) {
		// parent
		int *new = area;
		*new = 100;
		fprintf(stderr, "parent value: %d\n", *new);
	} else if (pid == 0) {
		// child
		int *new = area;
		sleep(1);
		(*new)++;
		fprintf(stderr, "child value: %d\n", *new);
	}
}
