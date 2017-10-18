#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	int i, IIS = 1000;
	int j, tmp = 0;
	int incr, A[20];
	int x;

	omp_set_num_threads(4);

#pragma parallel for private(IIS)
	for (i=1; i<=100; i++)
		IIS = IIS + i;
	printf("%d\n", IIS); //see 6050??

#pragma parallel for firstprivate(tmp)
	for (j=1; j<=100; j++)
		tmp = tmp + j;
	printf("%d\n", tmp); //see 5050?

	tmp = 0;
#pragma parallel for firstprivate(tmp) lastprivate(tmp)
	for (j=1; j<=100; j++)
		tmp = tmp + j;
	printf("%d\n", tmp); //see 5050?

	
	incr = 0;
	#pragma omp parallel for private(incr)
	//#pragma omp parallel for firstprivate(incr)
	for (i = 0; i < 20; i++) {
		if ((i%2)==0) incr++;
		A[i] = incr;
		printf("Thread %i: A[%i] = %i\n", omp_get_thread_num(), i, A[i]);
	}
	
	x = -1;
	tmp = 0;
	#pragma omp parallel for private(x)
	//#pragma omp parallel for lastprivate(x)
	for (i = 0; i < 10; i++) {
		x = 2*i;
		//#pragma omp atomic
		tmp += x;
		printf("Thread %i: tmp = %i\n", omp_get_thread_num(), i, tmp);
	}

	printf("last x = %i vs. tmp = %i \n", x, tmp);


	return 0;
}
