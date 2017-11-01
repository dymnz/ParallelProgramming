#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
	struct timeval start, end;
	gettimeofday( &start, NULL );
	
	if (argc != 3)
	{
		printf("usage: ./[.exe] [input.txt] [output.txt] \n");
		exit(1);
	}
		
	struct rusage r_usage;
	
    int i, j, k, n;
	double **a;
	double d;

	FILE *fp;
	fp = fopen(argv[1], "r");
	
	fscanf(fp, "%d", &n);

	// input matrix
	a = malloc((n*2+1) * sizeof(double*));
	for (i=0; i<n*2+1; i++)
		a[i] = malloc((n*2+1) * sizeof(double));
	
	for (i=1; i<=n; i++)
        for (j=1; j<=n; j++)
			fscanf(fp, "%lf", &a[i][j]);
	
	fclose(fp);
	
	
	for (i=1; i<=n; i++)
        for (j=1; j<=2*n; j++)
            if (j == (i+n))
                a[i][j]=1;
	
    /************** partial pivoting **************/
    for (i=n; i>1; i--)
    {
        if (a[i-1][1] < a[i][1])
			for (j=1; j<=n*2; j++)
			{
				d = a[i][j];
				a[i][j] = a[i-1][j];
				a[i-1][j] = d;
			}
    }
	
    printf("pivoted output: \n");
    for (i=1; i<=n; i++)
    {
        for (j=1; j<=n*2; j++)
            printf("%lf    ", a[i][j]);
        printf("\n");
    }
	
    /********** reducing to diagonal  matrix ***********/

	
    for (i=1; i<=n; i++)
    {
        for (j=1; j<=n*2; j++)
		{
			if (j != i)
			{
				d = a[j][i] / a[i][i];
				for (k=1; k<=n*2; k++)
					a[j][k] -= a[i][k]*d;
			}
		}
    }
    
	/************** reducing to unit matrix *************/
	for (i=1; i<=n; i++)
    {
		d = a[i][i];
        for (j=1; j<=n*2; j++)
            a[i][j] = a[i][j] / d;
    }

	
	fp = fopen(argv[2], "w");

    printf("your solutions: \n");
    for (i=1; i<=n; i++)
    {
        for (j=n+1; j<=n*2; j++)
		{
			printf("%lf    ", a[i][j]);
			fprintf(fp, "%lf    ", a[i][j]);
		}
        printf("\n");
		fprintf(fp, "\n");
    }

	fclose(fp);
	
			
	// measure the max. memory usage
	getrusage(RUSAGE_SELF,&r_usage);
	printf("Memory usage: %ld k-bytes\n",r_usage.ru_maxrss);
	
	// measure the time
	gettimeofday( &end, NULL );
	float timeuse = end.tv_sec - start.tv_sec + (float)(end.tv_usec - start.tv_usec) / 1000000;
    printf("time: %f s\n", timeuse);
    
    return 0;
}