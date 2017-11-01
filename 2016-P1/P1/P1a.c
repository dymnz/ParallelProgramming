#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <sys/time.h>

int main(int argc, char* argv[]) 
{ 
	struct timeval start, end;
	gettimeofday( &start, NULL );
	
    if(argc > 2 || argc < 2)
    {
        printf("usage: ./[exe] [N]");
        exit(-1);
    }

    /*******************************************************
    if(argc > 3 || argc < 3)
    {
        printf("usage: ./[exe] [num_thread] [N]");
        exit(-1);
    }
    ********************************************************/

    srand(1);
    
    int N = atoi(argv[1]);
    int sum = 0;
    int i;
    
    for(i = 1; i < N; i++)
    {
        double x = (double) rand() / RAND_MAX;
        double y = (double) rand() / RAND_MAX;
        if((x * x + y * y) < 1)
            sum++;
    }

    printf("PI = %f\n", (double) 4 * sum / (N - 1));
	
	gettimeofday( &end, NULL );
	float timeuse = end.tv_sec - start.tv_sec + (float)(end.tv_usec - start.tv_usec) / 1000000;
    printf("time: %f s\n", timeuse);

    return 0;
}
