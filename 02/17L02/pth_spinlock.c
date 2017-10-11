#include <pthread.h>

pthread_spinlock_t lock;

void spinlockex() {
	int i = 10000;
	while (i>0) {
		pthread_spin_lock(&lock);
		i--;
		pthread_spin_unlock(&lock);
	} 
}

int main() {
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	/*pthread_spin_init(&lock, PTHREAD_PROCESS_SHARED);*/
	spinlockex();
	pthread_spin_destroy(&lock);
	return 0;
}
