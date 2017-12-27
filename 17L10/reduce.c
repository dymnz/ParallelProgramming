#include <stdio.h>
#include "mpi.h"

int main (int argc, char *argv[]) {
	int rank, size, i, val, result;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	val = 2*rank+1;

  if (rank==0) printf("\nReduce Part:\n");
	printf("Process %d of %d has value = %d\n", rank, size, val);
	MPI_Reduce(&val, &result, 1, MPI_INT, MPI_SUM, 2, MPI_COMM_WORLD);
	printf("Process %d of %d sums up values = %d\n", rank, size, result);
	//sleep(1);
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank==0) printf("\nAllReduce Part:\n");
	result=0;
	MPI_Allreduce(&val, &result, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("Process %d of %d sums up values = %d\n", rank, size, result);

	MPI_Finalize();
	return 0;
}

