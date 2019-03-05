Feb 21 2019

Recall the bank example:

```c
pthread_mutex_lock(&lock);
balance++;
pthread_mutex_unlock(&lock);
```

### Two types of locks:
1. Sleeping Locks

2. Spin Locks


## Implementing spin lock

1. Wrong implmentation:

```c
int flag = 0;

lock() {
    while (flag == 1)
	;

    // This gap between testing and setting creates a race condition!

    flag = 1;
}

unlock() {
    flag = 0;
}
```


2. Correct implementation using atomic test_and_set hardware instruction:

```c
int flag = 0;

lock() {
    while(test_and_set(&flag))
	;
}

unlock() {
    flag = 0;
}
```
	
The atomic test_and_set hardware instruction essentially does the following:
    
```c
	int test_and_set(int *lock) {
	    int old = *lock;
	    *lock = 1;
	    return old;
	}
 ```   

Process Memory Layout
---
![virtual-memory](../img/kernel_img.svg)

Kernel data includes things like: 

- kernel stack for each process (8k for 32-bit and 16k for 64-bit)
- Linked list of task_struct (`tasks`)

Runs Queues
---


![run](../img/run_queue.svg)

Process State Transitions
---

![circle.png](../img/State_transition.svg)


Wait Queues
---

```c
struct waitqueue_head {
	spin_lock_t lock;
	list_head task_list;
}

struct waitqueue {
	task_struct *task;
	wq_func func; // call back function
	list_head task_list;
}
```

![wq_head](../img/wait_queues.svg)

TODO: copy from code

```c
void wait_event(wq, cond) {
	DEF_WAIT(wait);
    // forever loop here, because the process needs to be in the wait mode until the cond is true. It breaks out of the forever loop and stops waiting when the condition becomes true and finish_wait().
	for (;;) {
		prepare_to_wait(&wq, &wait, T_I);
		if (cond) {
			break;
		}
		if (signal_pending(current)) {
			break;
		}
		schedule();
	}
	finish_wait(&wq, &wait);
}

```

