#include <pthread.h>
#include <stdio.h>

int balance = 0;
pthread_mutex_t mut;
// thread 1
void* deposit(void *arg) {        
	int i;
	for(i=0; i<1e6; ++i) {

		//pthread_mutex_lock(&mut);
		// critical region
		++balance;
		//pthread_mutex_unlock(&mut);
	}
	int r = (int)arg;
	r *= 10;
	return (void *)r;
}

// thread 2
void* withdraw(void *arg) {        
	int i;
	for(i=0; i<1e6; ++i) {
		//pthread_mutex_lock(&mut);
		--balance;
		//pthread_mutex_unlock(&mut);
	}
	int r = (int)arg;
	r *= 10;
	return (void *)r;
}

// thread 3 - main thread
int main() {
	pthread_t t1, t2;       
 	pthread_mutex_init(&mut, NULL);

	// because integer(4 bytes) is never greater than the pointer (8bytes), it's a non-proper way of sending an integer by shoving it inside where a pointer's supposed to go. 
	// question? wouldn't the rest of 4 bytes be a garbage value and that would mess up the integer value?
	pthread_create(&t1, NULL, deposit, (void*)1);        
	pthread_create(&t2, NULL, withdraw, (void*)2);

	void *r1;
	void *r2;
	pthread_join(t1, &r1);
	pthread_join(t2, &r2);

	printf("t1 returned %d\n", (int)r1);
	printf("t2 returned %d\n", (int)r2);
	printf("balance = %d\n", balance);
}
