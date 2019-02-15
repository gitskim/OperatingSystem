Wed, Feb. 7th, 2018

How to handle aysnchronous model - synchronous programming is easier than asynchronous programming. 

* Don't be confused with multiprocesses or multicore systems. 

A thread ID is represented by the pthread_t data type.

* To compare thread ids: 

```c
#include <pthread.h>
int pthread_equal(pthread_t tid1, pthread_t tid2);
```

* To create a new thread:

```c
#include <pthread.h>
int pthread_create(pthread_t *restrict tidp,
const pthread_attr_t *restrict attr,
void *(*start_rtn)(void *), void *restrict arg);
```

### 11.5 Thread Termination

A single thread can exit in three ways:

1. The thread returns
2. The thread can be canceled by another thread
3. The thread calls pthread_exit()

```c
#include <pthread.h>
void pthread_exit(void *rval_ptr);
```

rval_ptr is available to other threads by calling join. pthread_join waits for the thread to terminate. 

```c
#include <pthread.h>
int pthread_join(pthread_t thread, void **rval_ptr);
```

### 11.5 Synchronization

1. Locking - making sure while one thread is modifying that value, no ther threads have access to that value.

* bus cycle: how long it takes to make a single read or write between the cpu and the external memory.

NOTE: UNSYNCHRONIZATION - The combination of incrementing the variable and making the decision of the value of the variable are not atomic, so it opens room for errors.

### 11.6.1 Mutexes

1. One thread is ABOUT TO use a variable

2. It sets a lock a.k.a. MUTEX - all other threads are blocked 

3. The thread uses a variable and done using it.

4. The lock is released.

5. The next thread on the line can use it. 

* pthread_mutex_t data type: a mutex variable

### 11.6.2. Deadlock

* Deadlock - if a thread tries to lock the same mutex twice, a thread will deadlock itself. 
