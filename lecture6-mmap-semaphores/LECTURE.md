Thursday, Feb. 1st, 2018

<code>pipe()</code> - only for related processes unless you use namedpipe (mkfifo). It creates a file that acts like a pipe, and you can use the file for communication tool between unrelated processes. 

<code>mmap()</code> - os will allocate a region of your address space and map it to the portion of the file which you passed in with a file descriptor. So when you write into the memory, it will be automatically written to the file. Whatever the file content is, you can read it from memory. 

#### NOTE: it's just another way to read and write to a file instead of using fread and fwrite.

e.g. Let's say you run execl and run another program by opening the program file and jump to that location. What OS actually does is it memory maps the code region into that executable file and jump to that location. question? I don't get it?

demand paging: late reading = memory mapping the text region of the program.


### Static Linking vs. Dynamic Linking

Linking Type|Description|Advantages|Disadvantages
---|---|---|---
Static linking|.a and .o files. You are pulling the real code.|Fast|Since it's manually pulling code, a lot of code gets repeated unnecessarily
Dynamic linking (memory mapping)|.so files. You are just putting the stub (fake) code during compilation and pull the real code during run time, if it's necessary|Slower|No repetivie code

```c
void *mmap(void *addr, size_t len, int prot,  int flag, int fd, off_t off);

//returns starting address of mapped region if ok, MAP_FAILED on error.
// addr - usually NULL, because you don't know where the memory starts. OS has no obligations to listen to your reqeusted addr.
// len - how much
// fd - file descriptor (IMPORTANT)
// prot - read only or write only
// flag - MAP_SHARED - to be shared by multiple processes
// off - offset into the file; where in the file you want to start.
```

NOTE: /dev/zero/ - is a pseudo file where you just read 0.  Everything undler /dev/ is a fake file. 

#### Anonymous mapping: 

```c
if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
```

```c
#include "he.h"
#include <fcntl.h>
#include <sys/mman.h>

#define NLOOPS 1000
#define SIZE sizeof(long)

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
```

### POSIX Semaphores

#### Binary: 0 or 1 vs. Counting Semaphores (multiple processes can have access to it)

##### NOTE: it's voluntary, so other processes can ignore semaphores. 

Semaphore value does not go below 0.

To create a semaphore:

#### UNRELATED PROCESSES - named semaphores
```c
sem_t *sem_open(const char *name, int oflag, ... /* mode_t mode,
                unsigned int value  */ );
Returns: Pointer to semaphore if OK, SEM_FAILED on error
```

To close the semaphore:
```c
int sem_close(sem_t *sem);
         Returns: 0 if OK, –1 on error
```

But that's not enough to get rid of the semaphore and delete it from memory:

```c
int sem_unlink(const char *name);
        Returns: 0 if OK, –1 on error
```
##### NOTE: O_CREAT needs to be used to create a semaphore and it can be referred to by other processes with a file name.

 
#### Related Processes - unnamed semaphores

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
        Returns: 0 if OK, –1 on error

int sem_destroy(sem_t *sem);
        Returns: 0 if OK, –1 on error
```

##### NOTE: You can't allocate sem_t on a stack, if you're using semaphores is to share it between processes. If it's on a stack, you can't share it between processes. So you allocate it on a shared memory. Also, in that case, you have to make it sharable. It's not the case if it's used for multi-threads.


#### To grab the key - decrement the semaphore
```c
int sem_trywait(sem_t *sem); // non-blocking
int sem_wait(sem_t *sem); // blocking function - if sem value was 3, it will return 2
int sem_timedwait(sem_t *restrict sem,
                  const struct timespec *restrict tsptr); // wait for a certain period of time
// Both return: 0 if OK, –1 on error
```

#### To increment the semaphore

```c
int sem_post(sem_t *sem);
        Returns: 0 if OK, –1 on error
```
