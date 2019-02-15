#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

char	buf[500000];

int
main(void)
{
	int		ntowrite, nwrite;
	char	*ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK);	/* set nonblocking */

	ptr = buf;
	while (ntowrite > 0) {
		errno = 0;

                // Wait until stdout is ready before we call write()
                fd_set write_fds;
                FD_ZERO(&write_fds); // initialize write_fds
                FD_SET(STDOUT_FILENO, &write_fds); // turn on file descriptor STDOUT_FILENO
                select(STDOUT_FILENO + 1, NULL, &write_fds, NULL, NULL); // blocks until STDOUT_FILENO is ready to be written to.

		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if (nwrite > 0) {
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}

	clr_fl(STDOUT_FILENO, O_NONBLOCK);	/* clear nonblocking */

	exit(0);
}
