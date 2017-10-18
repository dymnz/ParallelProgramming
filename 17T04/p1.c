/******************************************************************************
* DESCRIPTION:
* This example attempts to 4 sections in openMP to compute the sum of a array.
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 100

int main() {
	int i;
	float a[N], sum;

  omp_set_num_threads(4);
  
	/* Some initializations */
	for (i=0; i < N; i++)
		a[i] = i;
    
	//Use 4 sections to parallelize the copmutation
  #pragma omp parallel for
	for (i=0; i<N; i++) {
		sum += a[i];
	}

	printf("Sum = %f\n", sum);

	return 0;
}

