#include "mpi.h"
#include <stdio.h>
#define NELEM 25
int main(int argc, char** argv) {
	int numtasks, rank, source=0, dest, tag=1, i;
	typedef struct {
		float x, y, z;
		float velocity;
		int  n, type;
	} Particle;
	Particle     p[NELEM], particles[NELEM];
	MPI_Datatype particletype, oldtypes[2];
	int          blockcounts[2];

	/* MPI_Aint type used to be consistent with syntax of */
	/* MPI_Type_extent routine */
	MPI_Aint    offsets[2], extent, lb;

	MPI_Status stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	/* Setup description of the 4 MPI_FLOAT fields x, y, z, velocity */
	offsets[0] = 0;
	oldtypes[0] = MPI_FLOAT;
	blockcounts[0] = 4;
	MPI_Type_get_extent(MPI_FLOAT, &lb, &extent);
	offsets[1] = 4 * extent;
	oldtypes[1] = MPI_INT;
	blockcounts[1] = 2;
	/****************/
	MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &particletype);
	/****************/
	MPI_Type_commit(&particletype);

	if (rank == 0) {
		for (i=0; i<NELEM; i++) {
			particles[i].x = i * 1.0;
			particles[i].y = i * -1.0;
			particles[i].z = i * 1.0;
			particles[i].velocity = 0.25;
			particles[i].n = i;
			particles[i].type = i % 2;
		}
		for (i=0; i<numtasks; i++)
			MPI_Send(particles, NELEM, particletype, i, tag, MPI_COMM_WORLD);
	}
	MPI_Recv(p, NELEM, particletype, source, tag, MPI_COMM_WORLD, &stat);
	printf("rank= %d   %3.2f %3.2f %3.2f %3.2f %d %d\n", rank,p[3].x,
	       p[3].y,p[3].z,p[3].velocity,p[3].n,p[3].type);

	MPI_Type_free(&particletype);
	MPI_Finalize();

	return 0;
}

