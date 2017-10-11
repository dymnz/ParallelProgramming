#include <stdio.h>                                                              
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main() {
    int i;
    omp_set_num_threads(2);

 #pragma omp parallel for schedule(static)     
 //#pragma omp parallel for schedule(static,2)     
 //#pragma omp parallel for schedule(dynamic)     
 //#pragma omp parallel for schedule(dynamic,2)     
 //#pragma omp parallel for schedule(guided)     
 //#pragma omp parallel for schedule(guided,2)  
 //setenv/export OMP_SCHEDULE "dynamic,2"   
    for (i=0; i<10; i++) {
        printf("i=%d, thread id=%d\n", i, omp_get_thread_num());
        fflush(stdout);
    }
 
    return 0;
}

