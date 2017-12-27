#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

static int len=4;

int main (int argc, char *argv[]) {
	int rank, size, root, i;
	int sendbuf[len];
	int *recvbuf;

	root = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (i=0; i<len; i++)
		sendbuf[i] = len*rank+i;

	printf("Process %d has ", rank);
	for (i=0; i<len; i++)
		printf("%d ",sendbuf[i]);
	printf("\n");
	recvbuf = malloc(len*size*sizeof(int));

	MPI_Allgather(sendbuf, len, MPI_INT,
	              recvbuf, len, MPI_INT,
	              MPI_COMM_WORLD);
	printf("Allgather done!\n");

	printf("Process %d of %d gathers :\n", rank, size);
	for (i=0; i<len*size; ++i) {
		printf("%d ", recvbuf[i]);
	}
	printf("\n");

	MPI_Finalize();
	return 0;
}

