## TEST_AND_SET()
* test_and_set(): returns the value of flag and sets it to 1.

```c
lock() {
	while (test_and_set(&flag)
		yield();
}
```
* While flag is 1, yield();

## PROBLEM: next thread can't be determined

## SOLUTION: add a wait queue

```c
lock() {
	while (test_and_set(&flag)) {
		add myself to wait queue
		yield
	}
}

unlock() {
	flag = 0;
	if (any thread in wait queue) 
		wake up one wait thread
}
```
```c
typedef struct __mutext_t {
	int flag;
	queue_t *q;
} mutex_t;

lock() {
	whlie (test_and_set(&flag) {
		enqueue(m->q, self);
		yield();
	}
}

unlock() {
	flag = 0;
	if (!queue_empty(m->q)) {
		wake_up();
	}
}
```
* flag is to protect critical section.
* test_and_set() is protecting flag. 
* flag protection region will be given to user's critical section.

## Problem: wait queue is not protected: wake up call is lost and wrong thread gets lock. 

## Solution: protect wait queue

```c
typedef struct __mutext_t {
	int guard;
	int flag;
	queue_t *q;
} mutex_t;

lock() {
	while (test_and_set(&guard)
		;
	// wait for guard until it becomes 0 and set it to 1 and start doing critical things
	// the following can be inside the above while loop, but guard needs to be set back to 0 before yield(), so it's not in the while loop
	
	if (m->flag == 0) {
		// nobody is using the flag
		flag = 1;
		guard = 0;
	} else {
		// someone is using the flag. enqueue and yield
		enqueue(m->q, self);
		guard = 0;
		yield();
	}

	 /* while (test_and_set(&flag) {
		enqeue();
		guard = 0;
		yiled();
	}
	guard = 0; */
}
unlock() {
	while (test_and_set(m->guard) 
		;
	if (queue_empty(m->q) {
		m->flag = 0;
	} else {
		// if there is somethign in the queue, don't change the flag value and just give the flag to the new thread
		wakeup(deuque(m->q));
	}
	m->guard = 0;
	
	/* flag = 0;
	if (!queue_empty(m->q)) {
		wake_up();
	}
	m->guard = 0; */
}
```


```c
struct rwlock_t {
	int nreader;
	lock_t lock; // to protect critical region
};

write_lock(rwlock_t *l) {
	lock(&l->lock);
}

write_unlock(rwlock_t *l) {
	lock(&l->lock);
}

read_lock(rwlock_t *l) {
	// the point of reader lock is to let as many readers in as possible. So only the first one has to grab the lock and the last reader needs to release the lock.

	nreader++; 
	if (nreader == 1) {
		lock(&l->lock);
	}	
}

read_unlock(rwlock_t *l) {
	// the last reader needs to unlock
	nreader--;
	if (nreader == 1) {
		unlock(&l->lock);
	}
}
```

## Problem:
* nreader can get updated at the same time and if two readers come at the same time, nreader would be 2 and they don't have to acquire the lock and would start reading. 

## Solution: 
* Have another lock to protect nreader.  
```c
struct rwlock_t {
	int nreader;
	lock_t guard; // to protect nreader
	lock_t lock; // to protect critical region
};

write_lock(rwlock_t *l) {
	lock(&l->lock);
}

write_unlock(rwlock_t *l) {
	unlock(&l->lock);
}

read_lock(rwlock_t *l) {
	// the point of reader lock is to let as many readers in as possible. So only the first one has to grab the lock and the last reader needs to release the lock.

	lock(l->guard);
	nreader++;
	if (nreader == 1) {
		lock(&l->lock);
	}	
	unlock(l->guard);
}

read_unlock(rwlock_t *l) {
	// the last reader needs to unlock
	lock(l->guard);
	nreader--;
	if (nreader == 0) {
		unlock(&l->lock);
	}
	unlock(l->guard);
}
```

## PROBLEM: 

reader 1 came. writer came. the second reader can just go without acquirling the lock, although writer was there first. 

## SOLUTION:

Make another lock for both readers and writers to grab. 
* NOTE: the following solution is wrong again
```c
struct rwlock_t {
	int nreader;
	lock_t guard; // to protect nreader
	lock_t lock; // to protect critical region
	lock_t wlock; // second lock for ordering
};

write_lock(rwlock_t *l) {
	lock(&l->wlock);
	lock(&l->lock);
}

write_unlock(rwlock_t *l) {
	unlock(&l->lock);
	unlock(&l->wlock);
}

read_lock(rwlock_t *l) {
	// the point of reader lock is to let as many readers in as possible. So only the first one has to grab the lock and the last reader needs to release the lock.

	lock(&l->wlock);
	lock(l->guard);
	nreader++;
	if (nreader == 1) {
		lock(&l->lock);
	}	
	unlock(l->guard);
}

read_unlock(rwlock_t *l) {
	// the last reader needs to unlock
	lock(l->guard);
	nreader--;
	if (nreader == 0) {
		unlock(&l->lock);
	}
	unlock(l->guard);
	unlock(l->wlock);
}
```

* PROBLEM: wlock now protects critical section when there is already a lock for that
# Also, it beats the purpose of making multiple readers read critical section. 
# IMPORTANT (blow): 
### NOTE: there should be only one lock to protect the critical region and the rest of the locks are to protect data structures or something so they should be locked and unlocked wihtin one funciton.

* Solution: Use the wlock only for ordering purposes by decreasing the lock region

```c
struct rwlock_t {
	int nreader;
	lock_t guard; // to protect nreader
	lock_t lock; // to protect critical region
	lock_t wlock; // second lock for ordering
};

write_lock(rwlock_t *l) {
	lock(&l->wlock);
	lock(&l->lock);
	unlock(&l->wock);
}

write_unlock(rwlock_t *l) {
	unlock(&l->lock);
}

read_lock(rwlock_t *l) {
	// the point of reader lock is to let as many readers in as possible. So only the first one has to grab the lock and the last reader needs to release the lock.
	lock(&l->wlock);
	lock(l->guard);
	nreader++;
	if (nreader == 1) {
		lock(&l->lock);
	}	
	unlock(&l->guard);
	unlock(&l->wlock);
}

read_unlock(rwlock_t *l) {
	// the last reader needs to unlock
	lock(l->guard);
	nreader--;
	if (nreader == 0) {
		unlock(&l->lock);
	}
	unlock(l->guard);
}
```

# Producer Consumer
### sem_t mutex is for the race condition when mutliple producers or consumers filling in the spot or emptying the slot together

```c
struct lock {
	sem_t full;
	sem_t empty;
	sem_t mutex; 
}

sem_init(&full, 0, 0);
sem_init(&empty, 0, N);
sem_init(&mutex, 0, 1);

producer() {
	sem_wait(&empty);
	sem_wait(&mutex);
	// fill a slot
	sem_post(&mutex);
	sem_post(&full);
}

consumer() {
	sem_wait(&full);
	sem_wait(&mutex);
	// empty a slot
	sem_post(&mutex);
	sem_post(&empty);
}
```


# Kernel Synchronization

## Interrupt Handler
* It doesn't have to be reentrant
* Spin locks must be used instead of semaphores, because you don't want the interrupt handler to go to sleep
*

