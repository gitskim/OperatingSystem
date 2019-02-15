#include "he.h"
#include "pthread.h"

struct msg {
	int data;
	struct msg *next;
};

struct msg *workq;

pthread_cond_t condi = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *child(void *arg) {
	pthread_mutex_lock(&mutex);
	struct msg *mp = malloc(sizeof(struct msg));
	mp->data = 111;
	mp->next = workq;
	workq = mp;
	pthread_cond_signal(&condi);
	pthread_mutex_unlock(&mutex);
}

void *parent(void *arg) {
	pthread_mutex_lock(&mutex);
	while(workq == NULL) {
		pthread_cond_wait(&condi, &mutex);
	}
	printf("parent read %d from child\n", workq->data);
	pthread_mutex_unlock(&mutex);
}

int main() {
	pthread_t p, c;
	pthread_create(&c, NULL, child, NULL);
	pthread_create(&p, NULL, parent, NULL);

	pthread_join(p, NULL);
	pthread_join(c, NULL);
}
