## 15.2 Pipes

Oldest form of UNIX System IPC and are provided by all UNIX systems. 

* pipes can be used only between processes that have a common ancestor. 

1. A pipe is created by a rpcoess
2. The process calls fork()
3. The pipe is used between the parent and the child. 

```c
#include <unistd.h>

int pipe(int fd[2]);  > explnatian
```

When one end of a pipe is closed

1. when read()-ing from a pipe whose write end has been closed, read() returns 0 to indicate an end of file after all the data has been read. 
2. when write()-int to a pipe whose read end has been closed, the signal SIGPIPE is generated. If we either ignore the signal or catch it and return from the signal handler, write returns -1 with errno set to EPIPE. 

question? why would write() returns -1 when read() is closed?

```c
int main(int argc, char **argv) {
	int n;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[MAXLINE];
	FILE *fp;

	if (argc != 2) {
		perror("wrong usage");
	}

	if ((fp = fopen(argv[1], "r")) == NULL)
		perror("can't open");

	if (pipe(fd) < 0) 
		perror("pipe error");

	if ((pid = fork()) < 0) 
		perror("fork error");
	else if(pid >0) {// parent
		close(fd[0]); // close input reading
		// parent copies fopened file and write to output
		while (fgets(line, MAXLINE, fp) != NULL) {
			n = strlen(line);
			if (write(fd[1], line, n) != n) perror("write error");
		}

		if (ferror(fp))
			perror("fgets error");
		// close write
		close(fd[1]);

		if (waitpid(pid, NULL, 0) < 0)
			perror("waitpid failed");
	} else {
		// child
		close(fd[1]); //close write output
		// duplicate fd[0] to be file input 
		// question? I think this null check is unnecessary?
		// textbook: In this program, if standard input had not been opened by the shell, the fopen at the beginning of the program should have used descriptor 0, the lowest unused descriptor, so fd[0] should never equal standard input.
		// question: isn't stdin, stdout and stderr always open and their file descriptors are taken?
		if (fd[0] != STDIN_FILENO) {
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) // fd[0] is duplicated onto STDIN_FILENO
				perror("dup2 error to stdin");
			close(fd[0]);
		}

		if ((pager = getenv("PAGER")) == NULL)
			pager = "./hello"; // default pager value
		
		//what is the purpose of this if check? -> to remove the '/'
		if ((argv0 = strrchr(pager, '/')) != NULL) //strrchr = returns a pointer to the last occurrence of a character
			argv0++;
		else 
			argv0 = pager;
		//question? when opening a pager, do you need argv0? why do you need arguments?
		if (execl(pager, argv0, (char *)0) < 0)
			perror("execl error");
	}

}
```

## 7.6 Memory Layout Of a C Program

## 14.8 Memory-Mapped I/O

Memory-mapped I/O lets us map a file on disk into a buffer in memory. question? why "a buffer" in memory not just "in memory"? 

When we fetch bytes from the buffer, the corresponding bytes of the file are read. 

```c
#include <sys/mman.h>
void *mmap(void *addr, size_t len, int prot, int flag, int fd, off_t off);
```

The mmap() function asks to map length bytes starting at offset offset from the file (or other object) specified by the file descriptor fd into memory, preferably at address start. This latter address is a hint only, and is usually specified as 0. The actual place where the object is mapped is returned by mmap().

* return value - starting address of the mapped area
* addr - address where we want the mapped region to start. 
* prot - protection; PROT_NON or PROT_READ PROT_WRITE PRO_EXEC

The mapped meory is somewhere between the heap and the stack.

|READ & WRITE| mmap and memcpy|
|copy the data from the kernel's buffer to the application's buffer(read). Copy the data from the applications's buffer to the kernel's buffer (write) | copy the data directly from one kernel buffer mapped into our address space into another kernel buffer mapped into our address space. This cpying is a result of page fault handling (is a type of exception raised by computer hardware when a running program accesses a memory page that is not currently mapped by the memory management unit (MMU)) question? how can there be still copying when there is an exception?

Recall that the mmap function (Section 14.8) can be used to map portions of a file into the address space of a process. This is conceptually similar to attaching a shared memory segment using the shmat XSI IPC function. The main difference is that the memory segment mapped with mmap is backed by a file, whereas no file is associated with an XSI shared memory segment.

<Anoymous Memory Mapping)

* MAP_ANON
* the file descriptor is -1
* it's not associated with a pathname through a file descriptor

## 15.10 POSIZ Semaphores

The POSIX IPC is part of the POSIZ:XSI extension. 

IPC includes message queues, semphares sets and shared memory. 

Alternatives

|XSI Message Queues| UNIX domain sockets and POSIX message queues|
|---|---|
|XSI Semaphores | POSIX Semaphores|
|XSI Shared memory | mmap()|

Semaphores are like bouncers at a nightclub. It is to limit the number of consumers for a specific reousrce. 

Mutex: a mutex (mutual exclusion object) is a program object that is created so that multiple program thread can take turns sharing the same resource, such as access to a file.

 
 Mutual Exclusion:

 1. Lock and unlock operations are always performed by the same thread.
 2. Thread blocked on semaphore cannot be awakened by a signal.


 Synchronization:


 1. Lock and unlock operations performed by different threads.
 2. Threads blocked on a synchronization semaphore have three options:
 a) Signals can be caught and handled. 
 b) Signals can be deferred until the semaphore operation is complete. 
 c) Signals can be handled as though the code were unsemaphored.

 It restricts the number of simultaneous threads to a shared resource up to a maximum number known as the semaphore count. When a thread requests to access a resource, the semaphore counter is decremented.

 A mutex is like a key to a toilet. One person at a time can have the key and occupy the toilet. When finished, he gives the key to the next person waiting in line. On the other hand, a semaphore is like having a number of identical toilet keys for several available toilets. The number of keys represents the semaphore count. The count is decremented whenever people come in. When all toilets are occupied, the count becomes zero so no one can use any toilets. If someone leaves, the count is incremented and the next person in line can use that key and so on.


