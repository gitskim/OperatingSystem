#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define COPYINCR (1024*1024*1024) // 1GB
#define NLOOPS 10
#define SIZE sizeof(long)

static int update(long *ptr) {
	return ((*ptr)++);
}

int main(int argc, char** argv) {
	int fd, i, counter;
	pid_t pid;
	void *area;

	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SARED, MAP_ANON | MAP_SHARED, -1,, 0)) == MAP_FAILED) perror("mmap error");

	if ((pid = fork()) < 0) perror("fork error");

	else if (pid > 0) {
		// parent
		for (i = 0; i < NLOOPS; i+=2) {
			if ((counter = update((long *)area)) != i) perror("parent error");
		}
	} else {
		// child:
		for (i = 1; i < NLOOPS + 1; i += 2) {
			if ((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);
		}
	}

}
