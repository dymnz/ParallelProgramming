#include <stdio.h>                                                              
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main() {
    int tid;
    printf("Hello from serial.\n");
    /*omp_set_num_threads(4);*/
    printf("total number of threads = %d\n",omp_get_num_threads());
    printf("Thread number = %d\n",omp_get_thread_num());

#pragma omp parallel private(tid)      
    {
        tid = omp_get_thread_num();
        printf("Hello from parallel thread index=%d\n", tid);
        if (tid==0) {
            printf("Total number of threads = %d\n",omp_get_num_threads());  
        }                      
    }

    printf("Hello from serial again.\n");
    return 0;
}

