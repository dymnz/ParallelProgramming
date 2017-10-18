#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
void main() {
	int i, j;
	double array[2][10000];

	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel for ordered schedule(dynamic)
	for (i=0; i<10; i++) {
		#pragma omp ordered
		printf("Iteration %i from thread %i\n", i, omp_get_thread_num());
	}


	printf("for collapsing...\n");
	#pragma omp parallel for //private(j)//collapse(2)
	//#pragma omp parallel for collapse(2)
	for (i=0; i<2; i++) {
		//#pragma omp parallel for
		for (j=3; j<10; j++) {
			array[i][j] = i+j;
			printf("Compute array[%i][%i] from thread %i\n", i, j, omp_get_thread_num());
		}
	}
	return ;
}
