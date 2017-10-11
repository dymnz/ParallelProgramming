#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define THREADSNUM 8
int main() {
int   i, n, chunk;
float a[100], b[100], result;
double start, end;

/* Some initializations */
  n = 100;
  chunk = 10;
  result = 0.0;
  for (i=0; i < n; i++) {
    a[i] = i * 1.0;
    b[i] = i * 2.0;
  }

  omp_set_num_threads(THREADSNUM);
start = omp_get_wtime( ); 
#pragma omp parallel for \
  default(shared) private(i) \
  schedule(static,chunk) \
  reduction(+:result)  
  for (i=0; i < n; i++)
    result = result + (a[i] * b[i]);
  printf("Final result= %f\n",result);
end = omp_get_wtime( );  
printf("start = %.16g\nend = %.16g\ndiff = %.16g\n",  
          start, end, end - start);  

  return 0;
}

