#include "mpi.h"
#include <stdio.h>
#include <math.h>

double f(double a) {
	return( 2.0 / sqrt(1 - a*a) );
}

int main(int argc, char *argv[]) {
	int N;		// Number of intervals
	double w, x;		// width and x point
	int    i, myid, num_procs;
	double mypi, others_pi;

	MPI_Init(&argc,&argv);                      // Initialize
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  // Get # processors
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	N = atoi(argv[1]);
	w = 1.0/(double) N;

	mypi = 0.0;

	/* ---------------------------------------------------------
	   Every MPI process computes a partial sum for the integral
	--------------------------------------------------------- */
	for (i = myid; i < N; i = i + num_procs) {
		x = w*(i + 0.5);
		mypi = mypi +  w*f(x);
	}
	/* -----------------------------
	   Now put the sum together...
	----------------------------- */
	if ( myid == 0 ) {
		/* ----------------------------------------------------
		   Proc 0 collects and others send data to proc 0
		 ---------------------------------------------------- */
		for (i = 1; i < num_procs; i++) {
			MPI_Recv(&others_pi, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, NULL);
			mypi += others_pi;
		}

		printf("Pi = %d\n\n");   // Output...
	} else {
		/* ---------------------------------------------
		   The other processors send their partial sum to processor 0
		  --------------------------------------------- */
		MPI_Send(&mypi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
