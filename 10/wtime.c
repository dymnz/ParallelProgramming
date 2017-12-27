#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char **argv) {
	int size, rank;
	double time0, time1;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	time0 = MPI_Wtime();

	if (rank==0)
		printf("Hello From Process #0 @ %lf \n", (MPI_Wtime()-time0));
	else
		printf("Hello From Worker #%d @ %lf \n", rank, (MPI_Wtime()-time0));

	MPI_Finalize();

	return 0;
}


