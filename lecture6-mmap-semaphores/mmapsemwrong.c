#include "he.h"
struct Pack {
	char word[100];
	sem_t sem;
	int dex;
};

struct Pack *pack;
static int indx;

void *first(void *arg) {
	struct Pack *pack = arg;
	int dex = pack->dex;
	fprintf(stderr, "first dex: %d\n", pack->dex);
	(pack->word)[dex] = 'g';
	(pack->dex)++;
	sem_post(&pack->sem);
}

void *second(void *arg) {
	struct Pack *pack = arg;
	// saving locally before sem_wait will cache the value before increment
	int dex = pack->dex;
	fprintf(stderr, "second waits and dex: %d\n", pack->dex);
	sem_wait(&pack->sem);
	fprintf(stderr, "second dex: %d\n", pack->dex);
	(pack->word)[dex] = 'e';
	(pack->dex)++;
	sem_post(&pack->sem);
}

void *third(void *arg) {
	struct Pack *pack = arg;
	int dex = pack->dex;
	fprintf(stderr, "third waits and dex: %d\n", pack->dex);
	sem_wait(&pack->sem);
	fprintf(stderr, "third dex: %d\n", pack->dex);
	fprintf(stderr, "third started\n");
	(pack->word)[dex] = 't';
	(pack->dex)++;
	sem_post(&pack->sem);
}

int main() {
	pack = mmap(NULL, sizeof(struct Pack), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	memset(pack->word, 0, sizeof(pack->word));
	// nonzero pshared == shared
	// question? value 0 makes it no cap?
	// question? will the word be either fnu or fun?
	sem_init(&pack->sem, 1, 0);
	pack->dex = 0;
	/*pthread_t t1, t2, t3;
	  pthread_create(&t1, NULL, second, (void *)pack);
	  pthread_create(&t2, NULL, first, (void *)pack);
	  pthread_create(&t3, NULL, third, (void *)pack);

	  pthread_join(t1, NULL);
	  pthread_join(t2, NULL);
	  pthread_join(t2, NULL);*/

	pid_t pid;
	pid = fork();
	if (pid == 0) {
		// child
		fprintf(stderr, "child\n");
		third(pack);
	} else if (pid > 0) {
		// parent
		fprintf(stderr, "back to parent\n");
		first(pack);
		pid = fork();
		if (pid == 0) {
			// second child
			fprintf(stderr, "second child\n");
			second(pack);
		} else {

		// parent
		sleep(1);
		while (waitpid(-1, NULL, WNOHANG) > 0) {

		}
		fprintf(stderr, "word:%s\n", pack->word);
		}
	}
}
