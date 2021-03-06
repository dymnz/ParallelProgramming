/******************************************************************************
* FILE: mm.c * AUTHOR: Blaise Barney * LAST REVISED: 06/28/05
******************************************************************************/
#include <omp.h>
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

	/*** Initialize matrices ***/
	for (i=0; i<NRA; i++)
		for (j=0; j<NCA; j++)
			a[i][j]= i+j;

	for (i=0; i<NCA; i++)
		for (j=0; j<NCB; j++)
			b[i][j]= i*j;

	for (i=0; i<NRA; i++)
		for (j=0; j<NCB; j++)
			c[i][j]= 0;

	/*** Do matrix multiply sharing iterations on outer loop ***/
	/*** Display who does which iterations for demonstration purposes ***/

	for (i=0; i<NRA; i++) {
		for(j=0; j<NCB; j++)
			for (k=0; k<NCA; k++)
				c[i][j] += a[i][k] * b[k][j];
	}

	/*** Print results ***/
	printf("******************************************************\n");
	printf("Result Matrix:\n");
	for (i=0; i<NRA; i++) {
		for (j=0; j<NCB; j++)
			printf("%6.2f   ", c[i][j]);
		printf("\n");
	}
	printf("******************************************************\n");
	printf ("Done.\n");

	return 0;
}
