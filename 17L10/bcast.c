#include <stdio.h>
#include "mpi.h"

int main (int argc, char *argv[]) {
	int rank, size, value;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Process %d has original Bcast value = %d\n", rank, value);
	
	if (rank==0) value = 0;
	MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank==0)
		printf("\nProcess %d initializes Bcast value = %d\n\n", rank, value);

	printf("Process %d receives Bcast msg: value = %d\n", rank, value);

	MPI_Finalize();
	return 0;
}

