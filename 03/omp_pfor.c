#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N       1000000
#define CHUNKSIZE   100

int main(){
int i, chunk;
double a[N], b[N];

/* Some initializations */
chunk = CHUNKSIZE;
for (i=0; i < N; i++)
  a[i] = b[i] = i * 2.0;

#pragma omp parallel for \
    shared(a,b,chunk) private(i) \
    schedule(dynamic,chunk)
  for (i=0; i < N; i++)
    a[i] = a[i] + b[i];

  return 0;
}
