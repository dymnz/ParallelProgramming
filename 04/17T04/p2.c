/******************************************************************************
* DESCRIPTION:
*   This program intends to use reduction in openMP for PI calculation.
*   Please set thread number 16
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

	start_time = omp_get_wtime();
 
  omp_set_num_threads(16);
  
  #pragma omp parallel
  {
		#pragma omp single
		printf("Number of threads = %d\n",omp_get_num_threads());
  
		//Use reduction to calculate subsum
		#pragma omp parallel for reduction(+:sum) private(x)
		for (i=1; i<=n; i++) {
			x = (i-0.5)*step;
			sum = sum + 4.0/(1.0+x*x);
		}
		pi = step * sum;

		run_time = omp_get_wtime() - start_time;

		printf("pi with %ld steps is %lf in %lf seconds\n\n", n, pi, run_time);
	}
	return 0;
}


