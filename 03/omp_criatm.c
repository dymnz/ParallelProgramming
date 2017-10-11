#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
  int x, count, i;
  x = 0;
  count = 0;

#pragma omp parallel shared(x) 
{
  // try remove this statement and see what happens
  #pragma omp critical 
  x = x + 1;
}  

  printf("critical section done. x=%d\n", x);

#pragma omp parallel private(i)
{
  // try remove this statement and see what happens
  //#pragma omp atomic 
  for (i=0; i<100; i++)
  #pragma omp atomic 
    count++;
}
  printf("atomic done. count=%d\n", count);

  return 0;
}
