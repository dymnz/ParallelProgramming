/******************************************************************************
* DESCRIPTION:
*   This program intends to use reduction in openMP for PI calculation.
*   Requirement: 1. set thread number to 16 by runtime routine
*                2. use reduction peration properly
*                3. take care of data (storage) attibutes
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

double step;
int main (int argc, char *argv[]) {
	double start_time, run_time;
	double x, pi, sum = 0.0;
	int i, n = atoi(argv[1]);
	step = 1.0/(double)n;

	#pragma omp parallel
	{

		start_time = omp_get_wtime();

		//only one thread to print out this
		printf("Number of threads = %d\n",omp_get_num_threads());

		//Use reduction to calculate subsum
		for (i=1; i<=n; i++) {
			x = (i-0.5)*step;
			sum = sum + 4.0/(1.0+x*x);
		}

		pi = step * sum;
		run_time = omp_get_wtime() - start_time;
	} //end of omp


	printf("pi with %ld steps is %lf in %lf seconds\n\n", n, pi, run_time);

	return 0;
}


