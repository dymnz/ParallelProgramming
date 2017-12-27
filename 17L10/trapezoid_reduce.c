#include <mpi.h>
#include <stdio.h>

#define MPI_MATH MPI_DOUBLE
typedef double math_t;


math_t f(math_t x) {
	return x * x;
}

int main(int argc, char* argv[]) {
	int rank, size;

	int a = 2;
	int b = 25;
	int n = 1024;
	math_t h = (b - a) / (float)n;

	MPI_Init(&argc, &argv);      /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */

	int n_start, n_end, slice;
	slice = (n + (size - 1)) / size;
	n_start = slice * rank + 1;
	n_end = n_start + slice - 1;

	if (n_end >= n)
		n_end = n - 1;

	math_t x = a + n_start * h;
	math_t integral_segment = 0.0;

	int i;
	for (i = n_start; i <= n_end; ++i) {
		integral_segment += f(x);
		x += h;
		if (x > b) {
			x = b;
		}
	}

	printf("For process %5d start %5d end %5d segment_sum: %lf\n",
	       rank, n_start, n_end, integral_segment);

	math_t integral_result;
	MPI_Reduce(
	    &integral_segment,
	    &integral_result,
	    1,
	    MPI_MATH,
	    MPI_SUM,
	    0,
	    MPI_COMM_WORLD
	);
	
	if (rank == 0) {
		integral_result = (f(a) + f(b)) / 2 + integral_result;

		integral_result *= h;

		printf("With n = %d trapezoids, our estimate\n",
		       n);
		printf("of the integral from %d to %d = %lf\n",
		       a, b, integral_result);
	}

	MPI_Finalize();

	return 0;
}