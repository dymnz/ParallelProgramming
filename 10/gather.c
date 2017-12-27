#include <stdio.h>
#include "mpi.h"

int main (int argc, char *argv[]) {
	int rank, size, i, num[100];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Gather(&rank, 1, MPI_INT, num, 1, MPI_INT, 2, MPI_COMM_WORLD);

	if (rank==2) {
		printf("Process %d of %d gather from other process:\n", rank, size);
		for (i=0; i<size; i++) {
			printf("%4d", num[i]);
			if ((i+1)%4==0) printf("\n");
		}
		printf("\n");
	}

	MPI_Finalize();
	return 0;
}

