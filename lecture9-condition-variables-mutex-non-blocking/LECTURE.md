Tue 2/13/18

### Condition Variables - THE MOST IMPORTANT
* Addition to a mutex to make mutex more useful
* Used with mutex

### HW3
* a pool of threads - workers
* the main thread - keeps calling accept - once accept returns, it passes the file descriptor to another worker thread and keeps going back to accept. 

```c
#include <pthread.h>
#include "he.h"

struct msg {
	struct msg *m_next;
}

// head of the list
struct msg *workq;

pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg() {
	struct msg *mp;
	for (;;) {
		pthread_mutex_lock(&qlock);
		
		// critical region starts with pthread_mutex_lock and it ends with pthread_mutex_unlock
		// if the head of the list is empty
		while (workq == NULL) {
			pthread_cond_wait(&qready, &qlock); // atomic operations - 1. the calling thread is on the list of threads waiting for the condition 2. unlocks the mutex, 3. condition checked 4. the thread goes to sleep waiting for the condition to change. 
			// when pthread_cond_wait returns, teh mutex is locked. 
		}

		// if the head of the list is not empty, it copies the head of the list to the local variable mp
		mp = workq;

		// It's giving workq's first one in the list to workq and keepting the rest of the list to workq.
		workq = mp->mp_next;

		pthread_mutex_unlock(&qlock);
	}
}

void enqueue_msg(struct msg *mp) {
	// critical region starts
	pthread_mutex_lock(&qlock);

	mp->m_next = workq;
	// adding an item to the workq
	workq = mp;

	// it's always safe to signal before unlock to maue sure signal doesn't get lost. 
	// In the text book 
	pthread_cond_signal(&qready);

	pthread_mutex_unlock(&qlock);

// In the textbook, the last two lines are written in reverse order:
//     pthread_mutex_unlock(&qlock);
//     pthread_cond_signal(&qready);

// Taking pthread_cond_signal() call out of the mutex region is
// allowed (and better) in this particular case, but it is not
// always safe to do so - that's why it's not allowed in Java.
}
```

In Java Mutex and condition variable embedded in every object. 
<code>notify()</code> is illegal without holding mutex in Java

... more info omitted ... (30m into the audio)

## Nonblocking I/O and I/O multiplexing

### Nonblocking I/O

To make "slow systems calls non-blocking

1. call <code>open()</code> with <code>O_NONBLOCK</code>

e.g. open() will return immediately

2. call <code>fcntl()</code> with <code>O_NONBLOCK</code>

e.g. accept() or read() can be used with a file descriptor fcntl'ed. Then they will fail and errno set to EAGAIN. 


NOTE: <code>fcntl()</code> is a misc function that does a lot of small things. 

```c
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
```
