/******************************************************************************
* FILE: mm.c * AUTHOR: Blaise Barney * LAST REVISED: 06/28/05
******************************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NRA 4                 /* number of rows in matrix A */
#define NCA 4                /* number of columns in matrix A */
#define NCB 1                 /* number of columns in matrix B */

int main() {
	int	tid, nthreads, i, j, k, chunk;
	double	a[NRA][NCA],           /* matrix A to be multiplied */
	        b[NCA][NCB],           /* matrix B to be multiplied */
	        c[NRA][NCB];           /* result matrix C */

	chunk = 10;                    /* set loop iteration chunk size */

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if (rank == 0) {
		/*** Initialize matrices ***/
		for (i = 0; i < NRA; i++)
			for (j = 0; j < NCA; j++)
				a[i][j] = i + j;

		for (i = 0; i < NCA; i++)
			for (j = 0; j < NCB; j++)
				b[i][j] = i * j;

		for (i = 0; i < NRA; i++)
			for (j = 0; j < NCB; j++)
				c[i][j] = 0;
	}

	// Broadcast b
	// Scatter a

	/*** Do matrix multiply sharing iterations on outer loop ***/
	/*** Display who does which iterations for demonstration purposes ***/

	// Process {N} is in charge of {N}-row * b
	// Process {N} send result to process 0
	// 
	// TODO: Interlaced processing
	

	for (i = 0; i < NRA; i++) {
		for (j = 0; j < NCB; j++)
			for (k = 0; k < NCA; k++)
				c[i][j] += a[i][k] * b[k][j];
	}

	/*** Print results ***/
	printf("******************************************************\n");
	printf("Result Matrix:\n");
	for (i = 0; i < NRA; i++) {
		for (j = 0; j < NCB; j++)
			printf("%6.2f   ", c[i][j]);
		printf("\n");
	}
	printf("******************************************************\n");
	printf ("Done.\n");

	return 0;
}
