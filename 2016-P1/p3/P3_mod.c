#include <stdio.h>
#include <stdio.h>
#include <omp.h>

int binomial(int n, int k)
{
  if (k == 0 || k == n)
    return 1;
  else
  {
    int x, y;

    #pragma omp task shared(x)
    x = binomial(n - 1, k - 1);
    #pragma omp task shared(y)
    y = binomial(n - 1, k);

    #pragma omp taskwait
    return x + y;
  }
}

int main()
{
  int k, n;

  printf("Please input number n and k：");
  scanf("%d %d", &n, &k);
  {
    printf ("C(%d,%d)=%d\n", n, k, binomial(n, k));
  }
  return 0;
}
