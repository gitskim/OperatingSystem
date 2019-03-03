Feb 21 2019

When two threads try to modify a variable:

![unprotected](../img/unprotected.png)

If both threads of execution read the initial value of i before it is incremented, both threads increment and save the same value. As a result, the variable i contains the value 8 when, in fact, it should now contain 9.

To synchronize multi-threaded processes, we need to use locks. 

```c
pthread_mutex_lock(&lock);
balance++;
pthread_mutex_unlock(&lock);
```

### Two types of locks:
(1) Sleeping Locks
* When tyring to acquire a semaphore that is unavailable, the task is put onto a wait queue and goes to sleep. The processor is then free to execute other code.
* When the semaphore becomes available, one of the tasks on the wait queue is awakened so that it can then acquire the semaphore.
* It's ok for a process to go to sleep while holding the lock. 
* e.g.: semaphores, mutexes


(2) Spin Locks
* When a trying to acquire a spin lock while it is already held, the thread constantly checks if the lock is ready for availability while waiting (spin-waiting). 
* As soon as the lock becomes available, the thread can immediately acquire the lock and continue.
* The spinning prevents more than one thread of execution from entering the critical region at any one time.
* It's not ok for a process to go to sleep while holding the lock because it will exhaust cpu's. 


NOTE: The main difference between sleeping locks and spin locks is that acquiring a spin lock means it's constantly checking for its availability while sleeping locks do not do constant cpu-exhausting check. 



## Simple implementation of a lock
```c
int flag = 0;

// checking the value of the flag and changing it 
lock() {
    while (flag == 1) {
        // wait;
    }
    // GAP
    flag = 1;
}

unlock() {
    flag = 0;
}
```
* Drawback: there is a gap in the lock() function, which enables two threads to go at the same time. 

* Solution: **test_and_set(&flag)** to check the value of the flag and change it at the same time


NOTE: Being blocked simply means you are not in the runqueue. After runqueue becomes empty, it looks at the wait queue and puts a task from the wait queue on the runqueue (this information belongs to runqueue section)

# Kernel
![virtual-memory](../img/kernel_img.svg)

* Kernel code is shared by all processes. 
* Virtual memory is divided into pages. Each page size is 4K. 
* **Frame** is the **page** from the physical memory perspective (What you want to store is a page and where you want to store is a frame).

# Kernel Memory Region
* 8k size of per-process kernel stack - used while in kernel
* Recursion in kernel code is not preferred. 
* task_struct is about 1-2k. Since it's so big, it's never in the stack but allocated in the heap. Both threads and processes are represented as task_struct. 

```c
struct task_struct {
	struct list-head *rcu_node_entry;
}
```

![ts](../img/linux_task_structure.svg)

* NOTE: task_struct doesn't have the **list_head**. Instead, init_task of pid 0, a swapper acts like a head. 

* **current**, which is a macro, provides a pointer to the task struct. 
* When context switch or system mode changes, task_struct needs to be immediately available. Where should the pointer to task_struct be saved?

* The stack pointer gets automatically set to point to the kernel stack by hardware. 

* To get to the bottom of the kernel stack, the following assembly code is used:

```c
movl $-8192 %eax
andl %esp %eax
```

![stack_register](../img/stack_register.svg)

* No matter where the current stack pointer is, it can always go to the bottom of the stack by killing 13 0's at the end of the address, since the stack size is 8k == 2^13.

# Runs Queues

![run](../img/run_queue.svg)

* The highest priority task will run from a run queue (depending on a scheduler)
* Being on run queues doesn't necessarily mean it will run

# Wait Queues
* NOTE: we can't have a run queue structure for wait queues because we would end up having way too many list_head structs in task_struct, so we need a different structure for wait queues.
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

* wait queue head is the starting point for wait queues. And each wait_queue points to a task_struct.
* There are different kinds of wait queues such as lock wait queues, I/O wait queues, so if a process is waiting on I/O, it'd be put on the I/O wait queue. 

# TASK_RUNNING

![circle.png](../img/State_transition.svg)
* TASK_RUNNING - misnomer. It means a task is runnable. 
* schedule(): entry point to the scheduling function in the kernel.
* e.g. Timer interrupt fires and the timer interrupt updates the kernel process time. Then the handler calls schedule(), which calls pick_next_task(). After that context_switch() gets called, a new process gets to run
* Kernel keeps calling schedule() all the time. 

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

// another way of implementing wait_event 
do {
	DEFINE_WAIT(__wait);
	for (;;) {
		prepare_to_wait(&wq, &__wait, T_I);
		if (cond) {
			break;
		}
		if (!signal_pending(current)) {
			schedule();
			continue;	
		}
		ret = -ERESTARTSYS;
		break;
	}
	finish_wait(&wq, &wait);
} while (0)
```

```c
void prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state) {
	unsigned long flags;

	wait->flags &= ~WQ_FLAG_EXCLUSIVE;
	spin_lock_irqsave(&q->lock, flags);
	if (list_empty(&wait->task_list))
		__add_wait_queue(q, wait);
	set_current_state(state);
	spin_unlock_irqrestore(&q->lock, flags);
}
```

<code>prepare_to_wait()</code> changes the process state to either T_I or T_U. It also adds the task back to the wait queue if necessary. 

Since <code>prepare_to_wait()</code> already locks that's attached to the queue_head, it's unnecessary to have a custom lock before using this function.

```c
typedef struct __wait_queue wait_queue_t;

struct __wait_queue {
	unsigned int flags;
#define WQ_FLAG_EXCLUSIVE	0x01
	void *private;
	wait_queue_func_t func;
	struct list_head task_list;
};
```

wait queue's descriptor address is stored in the private field.
