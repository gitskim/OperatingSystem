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

	// IMPORTANT: destroy the semaphore at the end.
	sem_destroy(&noname);
}
