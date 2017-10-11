#include <stdio.h>                                                              
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#define CHUNKSIZE 5
#define N     20

int main() {

int i, chunk;
float a[N], b[N];

/* Some initializations */
for (i=0; i < N; i++)
  a[i] = b[i] = i * 1.2;
chunk = CHUNKSIZE;

#pragma omp parallel shared(a,b,chunk) private(i)
{
  #pragma omp for schedule(static)
  for (i=0; i < N; i++) { 
    a[i] = a[i] + b[i];
    printf("%d - %d \n", omp_get_thread_num(), i);
  }
  printf("Thread %d done\n", omp_get_thread_num());   
}  /* end of parallel section */

  return 0;
}
