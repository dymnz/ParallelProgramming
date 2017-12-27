#include <stdio.h>
#include "mpi.h"

int main (int argc, char *argv[]) {
	int rank, size, len;
	char  name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Process %d\n",rank); //fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD); // collective call for synchronization
	sleep(1);

	MPI_Get_processor_name(name, &len);
	printf("Hello world! Process %d of %d on %s\n", rank, size, name); //fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD); // collective call for synchronization

	MPI_Finalize();
	return 0;
}

