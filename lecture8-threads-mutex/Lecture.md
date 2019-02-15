# POSIX Threads

## Create, Exit, Join

Threads - two processes that share the address space.

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

// it's a wait() for threads
int pthread_join(pthread_t thread, void **retval);

// thread exit function will just terminate the thread, even if it's in the middle of doing something. 
void pthread_exit(void *rval_ptr);
```
### NOTE: they have share everything except STACK. The thread starts fresh when the function starts and each thread will have their own stack. Therefore, the new thread does not have access to the main thread's STACK. 

### NOTE: unlike fork(), threads don't share history. The thread starts fresh in a function and that's when its history starts.

It calls <code>clone()</code> under the hood. 

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
