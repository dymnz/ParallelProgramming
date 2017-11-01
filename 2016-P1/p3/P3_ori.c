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
       x = binomial(n-1, k-1);
       y = binomial(n-1, k);
       return x+y;
    }
}

int main()
{
    int k, n;

    printf("Please input number n and k：");
    scanf("%d %d", &n, &k);
    {
    printf ("C(%d,%d)=%d\n", n, k, binomial(n,k));
    }
    return 0;
}
