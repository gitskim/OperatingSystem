# POSIX Threads

## Create, Exit, Join

### Threads 

1. two processes that share the global and heap memory, file descriptors and the stack is shared. 
2. It doesn't depend on a processor, because it doesn't actually run different tasks at the same time. CPU will run one thread for a certain time and run another thread. 
3. Every thread has a thread ID.

To create a new thread:

```c
int pthread_create(pthread_t *restrict tidp, //a thread object to refer to the thread
		   // you can pass a flag to change the normal default behavior - advanced, so just pass NULL
		   const pthread_attr_t *restrict attr, 
		   // a function to pass - This is how the thread starts. The thread will be doing what's inside the function.
		   // it accepts (void *) and returns (void *)
		   void *(*start_rtn)(void *), 
		   // a parameter to the function above. 
		   // If this argument is allocated in the main thread's stack, make sure the main thread is alive during the time the thread needs to have access to this argument. 
		   void *restrict arg);
```

To terminate a thread:

0. pthread_cancel - send a cancellation request to a thread
1. pthread_join - places the thread in the detached state = once the thread dies, its resources can be recovered. 
2. pthread_exit - thread will exit. 

* After a canceled thread has terminated, a join with that thread using pthread_join(3) obtains PTHREAD_CANCELED as the thread's exit status. (Joining with a thread is the only way to know that cancellation has completed.)


```c
// it's a wait() for threads
int pthread_join(pthread_t thread, void **retval);

// thread exit function will just terminate the thread, even if it's in the middle of doing something. 
void pthread_exit(void *rval_ptr);
```
### NOTE:

1. they share everything except STACK. The thread starts fresh when the function starts and each thread will have their own stack. Therefore, the new thread does not overwrite the main thread's STACK. 

* But their stack can be shared by passing pointers to another thread. 

2. unlike fork(), threads don't share history. The thread starts fresh in a function and that's when its history starts.

3. It calls <code>clone()</code> under the hood.
 
NOTE: both of the following code snippets return 11 

```c
void *thr_fn1(void *arg) {
	printf("thread 1 returning\n");
	return ((void *)11);
}

void *thr_fn2(void *arg) {
	printf("thread 2 exiting\n");
	pthread_exit((void *)11);
}
```
Their exit status can be obtained by calling pthread_join.

```c
void *exit_status;
pthread_join(tid2, &exit_status);

int status = (long) exit_status;
```


```c 
#include <pthread.h>
#include <stdio.h>

int balance = 0;

// thread 1
void* deposit(void *arg) {        
	int i;
	for(i=0; i<1e7; ++i) {
		xxx_lock();
		// critical region
		++balance;
		xxx_unlock()'
	}
	int r = (int)arg;
	r *= 10;
	return (void *)r;
}

// thread 2
void* withdraw(void *arg) {        
	int i;
	// 1e7 is 10 mil
	for(i=0; i<1e7; ++i) {
		xxx_lock();
		--balance;
		xxx_unlock();
	}
	int r = (int)arg;
	r *= 10;
	return (void *)r;
}

// thread 3 - main thread
int main() {
	pthread_t t1, t2;        
	// because integer(4 bytes) is never greater than the pointer (8bytes), it's a non-proper way of sending an integer by shoving it inside where a pointer's supposed to go. 
	// question? wouldn't the rest of 4 bytes be a garbage value and that would mess up the integer value?
	pthread_create(&t1, NULL, deposit, (void*)1);        
	pthread_create(&t2, NULL, withdraw, (void*)2);

	void *r1;
	void *r2;
	pthread_join(t1, &r1); //&r1's type is void **
	pthread_join(t2, &r2);

	printf("t1 returned %d\n", (int)r1);
	printf("t2 returned %d\n", (int)r2);
	printf("balance = %d\n", balance);
}
```

### NOTE: it's very important that stack variables remain alive if you pass them to a thread from the main thread, so make sure the main thread does not die before the threads, or else, the threads will just refer to a garbage value. Another way to resolve this issue is allocate the value on the heap. 

You will not always see 0 as a result. 

What happens is it will load the value i into a register and increment it, but when it does it so many times with multiple threads, it's buggy. The following diagram explains what happens. 

[!thread](../img/threads.png)


When it runs threads, it doesn't actually run them at the same time, because there is only one CPU. What happens is it will run one thing for a certain period of time and then it will move onto the next thread. The value will only change, if it was stuck in a thread and move onto the next thread to excute it. But as for dual core processor (CPU), although it's just one CPU, it looks like there are two processors to OS.

The result is more likely to be correct if it's run by one processor than multi-processors, because one processor will finish the job and move onto the next one. 

In order to make it run by one processor

```c
taskset 1 ./a.out
taskset 4 ./a.out //NOTE: the nubmer is a bit vector
```

# Synchronization

## Mutex

To create a mutex,

```c
int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
```

To detroy a mutex,

NOTE: If the mutex wrapper object was allocated dynamically by <code>malloc()</code>, mutex needs to be destroyed before freeing the mutex wrapper object. 

```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

To lock a mutex in a BLOCKING way,

NOTE: If the mutex is already locked, the calling thread will block until the mutex is unlocked.

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
```

To lock a mutex in a non-blocking way,

```c
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

To lock a mutex conditionally and in a non-blocking way,

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
```

```c
#include "he.h"
#include <pthread.h>

struct wrapper {
	int count;
	int data;
	pthread_mutex_t mutex;
};

struct wrapper *alloc(int data) {
	struct wrapper *fp;
	// malloc the wrapper
	if ((fp = (struct wrapper *) malloc(sizeof(struct wrapper))) != NULL) {
		fp->data = data;
		fp->count = 1;
		pthread_mutex_init(&fp->mutex, NULL);
		return fp;
	}
	return NULL;
}

void *increase (void *fd) {
	struct wrapper *fp = (struct wrapper *)fd;
	pthread_mutex_lock(&fp->mutex);
	fp->data++;
	fp->count++;
	pthread_mutex_unlock(&fp->mutex);
	return NULL;
}

void release(struct wrapper *fp) {
	if (--fp->count == 0) {
		pthread_mutex_destroy(&fp->mutex);
		free(fp);
	}
}

int main() {
	pthread_t t1, t2;
	struct wrapper *fp = alloc(11);
	pthread_create(&t1, NULL, increase, fp);
	pthread_create(&t2, NULL, increase, fp);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("value: %d\n", fp->data);
	release(fp);
}
```

## Deadlock

How deadlocks occur?

1. When multiple threads hold a mutex in different order. 

SOLUTION: Lock the mutex in the same order. 


e.g.

```c
#include "he.h"
#include "pthread.h"

pthread_mutex_t readMutex;
pthread_mutex_t writeMutex;
static int value;
void *writee(void *arg) {
	// problem
	pthread_mutex_lock(&writeMutex);
	sleep(1);
	pthread_mutex_lock(&readMutex);

	value++;

	pthread_mutex_unlock(&writeMutex);
	pthread_mutex_unlock(&readMutex);
}
void *readd(void *arg) {
	// problem
	pthread_mutex_lock(&readMutex);
	sleep(1);
	pthread_mutex_lock(&writeMutex);

	value++;

	pthread_mutex_unlock(&writeMutex);
	pthread_mutex_unlock(&readMutex);
}

int main() {
	pthread_mutex_init(&readMutex, NULL);
	pthread_mutex_init(&writeMutex, NULL);
	pthread_t t1, t2;
	pthread_create(&t1, NULL, readd, NULL);
	pthread_create(&t2, NULL, writee, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
```

solution is the mutexes need to locked in teh same order in every thread. 


2. In a single threaded and single process program, when a lock is locked without being unlocked. 

e.g.

```c
pthread_mutex_t lock;

pthread_mutex_lock(&lock);
pthread_mutex_lock(&lock); // it will hang
```

## Reader-Writer Locks = shared-exclusive locks

* Only one thread can hold a reader-writer lock in write mode = exclusive-mode
* Multiple threads can hold a reader-writer lock in read mode = shared-mode	

## Semaphores
* sem_wait(S) - Wait until S > 0, then increments S by 1.
* sem_post(S) - No waiting. Increments S by 1 and wake up a waiting thread 
