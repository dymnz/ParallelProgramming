#include <stdio.h>
#include <omp.h>


int print_1() {
if(0)
	return -1;
else
{
	#pragma omp task
	printf("print_1 thread:%3d\n", omp_get_thread_num());
	#pragma omp taskwait

	return 1;
}
}

int main() {

	omp_set_num_threads(8);
/*
	#pragma omp parallel
	{
		#pragma omp single
		print_1();
	}
*/
	#pragma omp parallel
	{
		#pragma omp single
		printf("%d %d %d\n", 1, 2, print_1());
	}


	return 0;
}

