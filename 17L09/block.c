#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rank, bsize, *buf, recv;
	int nums[4] = {0,1,2,3};
	MPI_Status status;

	MPI_Init(&argc,&argv);                      // Initialize
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank==0)
		MPI_Send(&nums[rank],1,MPI_INT,4,0,MPI_COMM_WORLD);
	else if (rank==1) {
		MPI_Pack_size(1, MPI_INT,MPI_COMM_WORLD, &bsize);
		bsize += 2*MPI_BSEND_OVERHEAD;
		buf = (int *)malloc(bsize);
		MPI_Buffer_attach(buf,bsize+MPI_BSEND_OVERHEAD);
		MPI_Bsend(&nums[rank],1,MPI_INT,5,0,MPI_COMM_WORLD);
		MPI_Buffer_detach(&buf,&bsize);
		free(buf);
	} else if (rank==2)
		MPI_Rsend(&nums[rank],1,MPI_INT,6,0,MPI_COMM_WORLD);
	else if (rank==3)
		MPI_Ssend(&nums[rank],1,MPI_INT,7,0,MPI_COMM_WORLD);
	else if (rank==4) {
		MPI_Recv(&recv,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
		printf("Process %d receives msg from process %d: %d\n",
		       rank, status.MPI_SOURCE, recv);
	} else if (rank==5) {
		MPI_Recv(&recv,1,MPI_INT,1,0,MPI_COMM_WORLD,&status);
		printf("Process %d receives msg from process %d: %d\n",
		       rank, status.MPI_SOURCE, recv);
	} else if (rank==6) {
		MPI_Recv(&recv,1,MPI_INT,2,0,MPI_COMM_WORLD,&status);
		printf("Process %d receives msg from process %d: %d\n",
		       rank, status.MPI_SOURCE, recv);
	} else if (rank==7) {
		MPI_Recv(&recv,1,MPI_INT,3,0,MPI_COMM_WORLD,&status);
		printf("Process %d receives msg from process %d: %d\n",
		       rank, status.MPI_SOURCE, recv);
	} else {
		;
	}

	MPI_Finalize();
	return 0;
}
