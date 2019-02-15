#include "he.h"
#include <pthread.h>

int done = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void threadExit() {
	pthread_mutex_lock(&mut);
	done = 1;
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&mut);
}

void *child(void *arg) {
	printf("child\n");
	threadExit();
	return NULL;
}

void threadJoin() {
	pthread_mutex_lock(&mut);
	while (done == 0) {
		pthread_cond_wait(&c, &mut);
	}
	pthread_mutex_unlock(&mut);
}

int main() {
	printf("parent about to being\n");
	pthread_t ch;
	pthread_create(&ch, NULL, child, NULL);
	threadJoin();
	printf("parent end\n");
	
}
