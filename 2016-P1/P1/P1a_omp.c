#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

#define THREAD_NUM 8

int main(int argc, char* argv[])
{
    struct timeval start, end;
    gettimeofday( &start, NULL );

    if (argc > 2 || argc < 2)
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

    omp_set_num_threads(THREAD_NUM);

    //double x[THREAD_NUM], y[THREAD_NUM];
    double x, y;

    int sum_list[THREAD_NUM];
    
    unsigned int seed;

    #pragma omp parallel for \
    reduction(+:sum) \
    private(x, y, seed) \
    schedule(static)
    for (i = 1; i < N; i++)
    {
        /*
        x[omp_get_thread_num()] = (double) rand_r(&seed_list[omp_get_thread_num()]) / RAND_MAX;
        y[omp_get_thread_num()] = (double) rand_r(&seed_list[omp_get_thread_num()]) / RAND_MAX;*/
        x = (double) rand_r(&seed) / RAND_MAX;
        y = (double) rand_r(&seed) / RAND_MAX;
        
        //printf("%10.10lf %10.10lf\n", x, y;
        if ((x * x + y * y) < 1) {
            //#pragma omp critical
            sum++;
            //sum_list[omp_get_thread_num()]++;
        }
    }

    // for (i = 0; i < THREAD_NUM; ++i)
    //     sum += sum_list[i];


    printf("PI = %f\n", (double) 4 * sum / (N - 1));

    gettimeofday( &end, NULL );
    float timeuse = end.tv_sec - start.tv_sec + (float)(end.tv_usec - start.tv_usec) / 1000000;
    printf("time: %f s\n", timeuse);

    return 0;
}
