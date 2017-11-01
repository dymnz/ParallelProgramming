#include <stdio.h>
#include <stdio.h>
#include <omp.h>

int binomial(int n, int k)
{
  int x, y;
  if (k==0 || k==n)
    return 1;
  else
    {
       #pragma omp task shared(x) firstprivate(n, k)
       x=binomial(n-1, k-1);

       #pragma omp task shared(y) firstprivate(n, k)
       y=binomial(n-1, k);

       #pragma omp taskwait
       return x+y;
    }
}

int main()
{
    int k, n;

    printf("Please input number n and k：");
    scanf("%d %d", &n, &k);

    omp_set_num_threads(8);
#pragma omp parallel shared(n)
    {
#pragma omp single
        printf ("C(%d,%d)=%d\n", n, k, binomial(n,k));
    }
    return 0;
}
