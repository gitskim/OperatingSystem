#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT    5555
#define MAXMSG  512

int read_from_client (int filedes) {
	char buffer[MAXMSG];
	int nbytes;

	nbytes = read (filedes, buffer, MAXMSG);
	if (nbytes < 0) {
		/* Read error - client closes the connection */
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
		for (i = 0; i < FD_SETSIZE; ++i)
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
