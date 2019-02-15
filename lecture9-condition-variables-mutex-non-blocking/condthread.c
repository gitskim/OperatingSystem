#include <pthread.h>
#include "he.h"

struct msg {
	struct msg *m_next;
}

// head of the list
struct msg *workq;

pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg() {
	struct msg *mp;
	for (;;) {
		pthread_mutex_lock(&qlock);
		
		// critical region starts with pthread_mutex_lock and it ends with pthread_mutex_unlock
		// if the head of the list is empty
		while (workq == NULL) {
			pthread_cond_wait(&qready, &qlock); // atomic operations - 1. the calling thread is on the list of threads waiting for the condition 2. unlocks the mutex, 3. condition checked 4. the thread goes to sleep waiting for the condition to change. 
			// when pthread_cond_wait returns, teh mutex is locked. 
		}

		// if the head of the list is not empty, it copies the head of the list to the local variable mp
		mp = workq;

		// It's giving workq's first one in the list to workq and keepting the rest of the list to workq.
		workq = mp->mp_next;

		pthread_mutex_unlock(&qlock);
	}
}

void enqueue_msg(struct msg *mp) {
	// critical region starts
	pthread_mutex_lock(&qlock);

	mp->m_next = workq;
	// adding an item to the workq
	workq = mp;

	pthread_cond_signal(&qready);

	pthread_mutex_unlock(&qlock);
}
