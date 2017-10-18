#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
void main() {

	int i, n=100, chunk=10;
	double a[100], b[100], sum=0.0;
	omp_set_num_threads(NUM_THREADS);

	for (i=0; i<100; ++i) {
		a[i] = i*0.5;
		b[i] = i*0.7;
	}

	//#pragma omp parallel for default(shared) private(i)\
	#pragma omp parallel for default(shared)\
	reduction(+:sum) schedule(static,chunk)
	for (i=0; i< 100; i++)
		sum += (a[i]*b[i]);

	printf("Final result= %f\n",sum);
}
