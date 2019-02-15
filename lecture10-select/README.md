Thu Feb. 15th, 2018

This is what <code>cat</code> does:

```c
while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
	if (write(STDOUT_FILENO, buf, n) != n)
		err_sys("write error");
}
```

#### PROBLEM: If this program were to be used for the keyboard, it blocks on read. netcat reads from stdin and writes to the socket and reads the socket and writes to stdout, which would require a similar loop above, so would need two loops. How does netcat do the two loops at the same time? - <code>netcat</code> does I/O in non-blocking way. But if netcat is made in non-blocking and if it keeps checking all the file descriptors, it uses so much CPU power, which is not ideal.

#### SOLUTION: <netcat>select()</netcat> - read from multiple file descriptors in a non-blocking way efficiently.


```c
#include <sys/select.h>

// fd_set is an array of integers with bits that represet a file descriptor - details don't matter
int FD_ISSET(int fd, fd_set *fdset);

        Returns: nonzero if fd is in set, 0 otherwise

void FD_CLR(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_ZERO(fd_set *fdset);

// question? why does it have separate readfds and writefds?
// readfds - file descriptors to read from
// writefds - file descriptors to write to
// exceptfds - file descriptors for exceptions
// maxfdp1 - max file descirptor plus 1 - whatever file descriptor you passed in for reading + 1 e.g. 5 (b/c the file descriptor for readfds 4 + 1) - it's used in a for loop.
// NOTE: maxfdp1 is 1 greater than the actual file descriptor value, not the number of file descriptors. 

int select(int maxfdp1, fd_set *restrict readfds,
           fd_set *restrict writefds, fd_set *restrict exceptfds,
           struct timeval *restrict tvptr);

        Returns: count of ready descriptors, 0 on timeout,–1 on error
``` 
```c
char	buf[500000];

int main() {
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
```

<code>select()</code> will always get interrupted on signals/sigaction, even when the <code>SA_RESTART</code> flag was used.

One thread and select() can make a server a.k.a. hw3/lab7


```c
#define PORT    5555
#define MAXMSG  512

int read_from_client (int filedes) {
	char buffer[MAXMSG];
	int nbytes;

	nbytes = read (filedes, buffer, MAXMSG);
	if (nbytes < 0) {
		/* Read error. When the client closes connection */
		perror (&quot;read&quot;);
		exit (EXIT_FAILURE);
	} else if (nbytes == 0)
		/* End-of-file. */
		return -1;
	else {
		/* Data read. and print to stderr */
		fprintf (stderr, &quot;Server: got message: `%s'\n&quot;, buffer);
		return 0;
	}
}
```
```c
int main () {
	extern int make_socket (uint16_t port);
	int sock;
	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	size_t size;

	/* <span class="roman">Create the socket and set it up to accept connections.</span> */
	sock = make_socket (PORT); //socket() and bind() and return server socket
	if (listen (sock, 1) < 0) {
		perror (&quot;listen&quot;);
		exit (EXIT_FAILURE);
	}

	/* Initialize the set of active sockets. */
	FD_ZERO (&active_fd_set);
	// set socket to active_fd_set
	FD_SET (sock, &active_fd_set);

	while (1) {
		/* Block until input arrives on one or more active sockets. */
		read_fd_set = active_fd_set;
		// in the beginning, it only checks for the server sock.
		// select returns if there is an incoming connection, which means accept will not be blocking and ready to go.
		// FD_SETSIZE is the maximum number of file descriptors which is a sloppy way to call select to go through all of it.
		if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
			perror (&quot;select&quot;);
			exit (EXIT_FAILURE);
		} // select modifies for both server socket and client sockets. 
		//If something was sent to the client socket, we will read from it. 

		/* Service all the sockets with input pending. */
		for (i = 0; i < FD_SETSIZE; ++i ) {
			if (FD_ISSET (i, &read_fd_set)) {
				if (i == sock) {
					// server socket
					int new;
					size = sizeof (clientname);

					// accept is not going to block.
					new = accept (sock, (struct sockaddr *) &clientname, &size); 
					// accept returned a new client socket. 
					// adding the new client socket to active fd set.
					FD_SET (new, &active_fd_set);
				}
				else {
					// client socket
					// if something was sent to the client socket, it simply reads it and prints to stderr.
					if (read_from_client (i) < 0) {
						// when the client closed the connection, we close that socket
						close (i);
						// take the socket out of the active set
						FD_CLR (i, &active_fd_set);
					}
				}
			}
		}
	}
}
```

55 m
