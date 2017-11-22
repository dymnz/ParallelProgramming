#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2
void main() {
	int *ready = calloc(sizeof(int), 2);
	omp_set_num_threads(NUM_THREADS);

	#pragma omp parallel
	{
		printf("Thread %i is ready.\n", omp_get_thread_num());
		ready[omp_get_thread_num()] = 1;
		int neighbor = (omp_get_thread_num()+1) % omp_get_num_threads();

		while (ready[neighbor] == 0) {
			#pragma omp flush
		}

		printf("Thread %i is done.\n", omp_get_thread_num());
	}
	free(ready);
}
