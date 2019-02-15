Thursday, Feb. 1st, 2018

<code>pipe()</code> - only for related processes unless you use namedpipe (mkfifo). It creates a file that acts like a pipe, and you can use the file for communication tool between unrelated processes. Related means they have a vertial relationship meaning they are parent/child or grandparent/child relationship as opposed to being siblings.  

<code>mmap()</code> - os will allocate a region of your address space and map it to the portion of the file which you passed in with a file descriptor. So when you write into the memory, it will be automatically written to the file. Whatever the file content is, you can read it from memory. 

Question: Relationship between mmap and shared object?

#### NOTE: it's just another way to read and write to a file instead of using fread and fwrite.

e.g. Let's say you run execl and run another program by opening the program file and jump to that location. What OS actually does is it memory maps the code region into that executable file and jump to that location and starts write/reading on the memory where the file is mapped.

<code>Normal mmap()</code>: File-backed mapping maps an area of the process's virtual memory to files. UNRELATED PROCESSES

<code>Anonymous mmap()</code>: NOT backed by any file. It maps an area of the process's virtual memory. Its contents are initialized to zero. RELATED PROCESSES. SHARED mode makes it visible to both parent and its children.

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

File-backed mmap to read and write on the file.

```c
#include "he.h"
#include <sys/stat.h>

int main() {
	int fd;
	// if file was opened read only mode, writing to mmaped region of that file would fail. 
	fd = open("/home/archie/practice/file", O_RDWR);
	struct stat fInfo = {0};
	fstat(fd, &fInfo);
	char *p = mmap(0, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	for (int i = 0; i < fInfo.st_size; i++) {
		fprintf(stderr, "%c", *p);
		*p = 'z'; // if mmap is only PROT_READ, this line will seg fault.
		p++; 
	}
}
```

### POSIX Semaphores

question: are semaphores first come first serve? jae said he's not sure because the os can't tell which one came first

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

## If memory is shared - unnamed semaphores.

## If memory is not shared - named semaphores.
 
#### Related Processes - unnamed semaphores

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
        Returns: 0 if OK, –1 on error

int sem_destroy(sem_t *sem);
        Returns: 0 if OK, –1 on error
```

#### NOTE: You can't allocate sem_t on a stack, if you're using semaphores to share it between processes. If it's on a stack, you can't share it between processes. So you allocate it on a shared memory. Also, in that case, you have to make it sharable. It's not the case if it's used for multi-threads.


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

Unnamed, unshared semaphore used in threads

```c
#include "he.h"

// unnamed semaphore that will be shared between threads.
sem_t noname;

void *handler (void *arg) {
	printf("handler 1\n");
	sem_wait(&noname);
	(*((int *)arg))++;
	printf("handler 1 val: %d\n", *((int *)arg));
	sem_post(&noname);
}

void *handler2 (void *arg) {
	printf("handler 2\n");
	sem_wait(&noname);
	(*((int *)arg))++;
	printf("handler 2 val: %d\n", *((int *)arg));
	sem_post(&noname);
}

int main() {
	int val = 4;
	sem_init(&noname, 0, 1);
	pthread_t t1, t2;
	pthread_create(&t1, NULL, (void *) &handler, (void *)&val);
	pthread_create(&t2, NULL, (void *) &handler2, (void *)&val);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("val at the end: %d\n", val);
	
	// IPORTANT - destroy the semaphore at the end
	sem_destroy(&noname);
}
```

Terrible example: Named semaphore used in parent-child processes

## Can you see what's wrong?

```c
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
		sem_close(sem);
		sem_unlink(name);

	} else if (pid > 0) {
		// parent
		sleep(1);
		sem_t *sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 1);
		sem_wait(sem);
		printf("parent waited\n");
		val++;
		printf("parent value %d\n", val); // the value should be 8
		sem_post(sem);

		sem_close(sem);
		sem_unlink(name);
	}
}

// NOTE: val can't be shared between child and parent.
```

