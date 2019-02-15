#include "he.h"
#include <errno.h>
#include <fcntl.h>

char buf[500000];

void set_fl(int fd, int flags) {
	int val;

	// F_GETFL - to read the file status flags for the fd
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) perror("fcntrl F_GETFL error");

	// turn on non blocking mode on that file descriptor bit you just acquired from fcntl
	val |= flags; 

	if (fcntl(fd, F_SETFL, val) < 0) perror("fcntrl F_SETFL error");
}

void clr_fl(int fd, int flags) {
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) perror("F_GETFL error");

	val &= ~flags;

	if (fcntl(fd, F_SETFL, val) < 0) perror("F_SETFL error");
}

int main() {
	int ntowrite, nwrite;
	char *ptr;
	// question? how to see write failed?
	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));

	fprintf(stderr, "Read%d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK);

	ptr = buf;

	while (ntowrite > 0) {
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if (nwrite > 0) {
			ptr += nwrite;
			ntowrite -=nwrite;
		}
	}

	// clear nonblocking
	clr_fl(STDOUT_FILENO, O_NONBLOCK);
}
