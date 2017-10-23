#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int i;
#pragma omp threadprivate( i )
void main() {

	omp_set_num_threads(2);

	printf("threadprivate?!...\n");
	i = -1;
	#pragma omp parallel
	{
		printf("[0] Parallel value = %i\n", i);
		i = 100+omp_get_thread_num();
		printf("[1] Parallel value = %i %ul\n", i, &i);
	}
	printf("[2] Serial value = %i %ul\n", i, &i);


	printf("copyin...\n");
	i=-1;
	#pragma omp parallel copyin( i )
	{
		printf("[3] Parallel value = %i\n", i);
		i = 100+omp_get_thread_num();
		printf("[4] Parallel value = %i\n", i);
	}
	printf("[5] Serial value = %i\n", i);


	printf("copyprivate...\n");
	i = -1;
	#pragma omp parallel
	//#pragma omp parallel copyin(i)
	{
		printf("Parallel value = %i from thread %i\n", i, omp_get_thread_num());
		#pragma omp single copyprivate(i)
		{
			i = 2;
		}
		printf("Parallel value = %i from thread %i\n", i, omp_get_thread_num());
	}
	printf("Serial value = %i from thread %i\n", i, omp_get_thread_num());

}
