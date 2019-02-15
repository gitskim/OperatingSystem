These are only for two threads.

# 1st attempt: One Flag [Test then set] - same-value-flag-based

## Drawback: both threads can go at the same time. 

```c
int flag = 0; // being shared 

// checking the flag and changing the flag. 
lock() {
	while (flag == 1) {
		// wait;
	}
	flag = 1;
}

// changing the flag
unlock() {
	flag = 0;
}
```

* Flag is used as like a semaphore to show that critical section can be done.

# 2nd attempt: Two Flags [Intent-based]

## Drawback: deadlock

```c
// changing my flag and looking at the other person's flag
lock() {
	flag[self] = 1; // I need lock
	while (flag[1-self] == 1) {
		// wait
	}
}

// changing my flag
unlock() {
	flag[self] = 0;
}
```

* If the flag values are changed at the same time, then it's a deadlock.

# 3rd attempt: Turn-based

## IT WORKS, but

## Drawback: 
1. The same thread can't lock repeatedly.
2. Inefficient because if one thread takes too long and the other thread takes very little, the other thread can't go multiple times. 

```c
int turn = 0;

// checking the other person's turn
lock() {
	while (turn == 1 - self) {
		// wait
	}
}

// changing the turn
unlock() {
	turn = 1 - self;
}
```

Difference between flag-based attempt and turn-based attempt:

Yes they both use one boolean value.

|Flag-based | Turn-based|
| ------------- |-------------|
|Both threads rely on the same value to either go or wait. It works like a semaphore. So one thread might not catch the value that has changed and starts executing a critical section while the other thread has already started. | The flag is used to notify which thread specifically should go. Each thread depends on a different value of the boolean value to go. That's why it works. |

NOTE: In a thread, changing the boolean value, if you waited for the value 0, you need to change that value to be 0 after waiting. So the value waited on is the same as the value to change to after waiting. 

e.g. waiting for 0 and changing turn to be 0.

```c
while (turn == 0) {
	// wait
}
// do critical stuff
// then change the turn
turn = 0;
```

# Peterson's Algorithm

## Requirements/Assuptinos:

1. Loads and stores are atomic - true

e.g. loadng and storing a variable like <code>int a = 3;</code>

2. Code lines execute in order - false
3. No hardware instructions needed - true

## Drawback:

1. They don't execute in order.

```c
// shared variables
int turn = 0;
int flag[2] = {0, 0};

lock() {
	flag[self] = 1; // I need lock
	turn = 1 - self; // it's my TRUN TO WAIT

	// wait
	while (flag[1-self] == 1 
		&& turn == 1 - self) {
		// checking the other persons' intent
		// and it's my turn 
		// NOTE: even if it's the other persons' turn
		// if she doesn't want to go, I can go
	}
	
}

unlock() {
	flag[self] = 0;
	// no need to change the turn, 
	// because it was already changed in the beginning
}
```

NOTE: they can both change the number at the same time. 
So if they try to change the number to the same value, it's dangerous. 
That's why turn-based is good, because they both try to change it to a different value. 

If the Peterson's algorithm was implemented in a way that setting the turn to mine and wait for the other person's turn, it can halt forever. 

```c
int turn = 0;
int flag[2] = {0, 0};

lock() {
	flag[self] = 1;
	turn = self;

	// wait
	while (flag[1-self] == 1 
		&& turn == 1 - self) {
		
	}
}

unlock() {
	// same
}
```

One thread goes and it can keep looping.

## Spin-wait

* On uniprocessor: spin-lock should not be used, because it's a waste of resources.
 
## Instead of spin-wait, sleep lock

```c
int flag;
lock() {
	while(test_and_set(&flag))
		yield();
}

unlock() {
	flag = 0;
}
```

* Problem: a lot of context switches = Thundering herd
* Problem 2: Starvation because there is no control over who gets the lock next. Fix needed to control who gets the lock next
* Solution: wait queue

## Sleep lock with wait queue

```c
lock() {
	while (test_and_set(&flag))
		// add myself to wait queue
		// yield
}

unlock() {
	flag = 0;
	if (any thread in wait queue)
		// wake up one wiat thread
}
```

* PROBLEM: lost wakeup 

Thread1 | Thread2
--- | ---
one. while (test_and_set(&flag)) | 2. call unlock()
five. add myself to queue 6. wait forever or until next unlock | 3. flag = 0; 4. if (any thread in wait queue) -> NO

* PROBLEM2: Wrong thread gets lock

Thread1 | Thread2 | Thread3
--- | --- | ---
one. lock(), 2. while (test_and_set()), 3. add myself to wait queue, 4. yield | 5. call unlock() 6. flag = 0, 7. if (thread in wait queue) wake up thread | 7. call lock(), 9. while (test_and_set(&flag))

NOTE: while thread 2 is waking up thread 1, thread 3 came in and held the lock after seeing the flag = 0.

All the problem has to do with wait queue not being protected. So the solution is to have another flag to protect the wait queue.

## Guard to protect the wait queue

```c
typedef struct __mutext_t {
	int flag; // 0: mutex is available, mutex is not available
	int guard; // to protect the wait queue
	queue_t *q; //queue of waiting threads
} mutex_t;

void lock(mutex_t *m) {
	while(test_and_set(m->flag)) {
		while(test_and_set(m->guard) {
			
		}
		// add myself to wait queue
		yeild();
	}
}

void unlock() {
	m->guard = 0;
	flag = 0;	
}
