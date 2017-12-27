/* trapv0.c -- Non-parallel Trapezoidal Rule, first version
 *
 * Input: None.
 * Output:  Estimate of the integral from a to b of f(x)
 *    using the trapezoidal rule and n trapezoids.
 */
#include <stdio.h>
float Trap(float, float, int, float);

int main(int argc, char** argv) {
	int         my_rank;   /* My process rank           */
	int         p;         /* The number of processes   */
	float       a = 2.0;   /* Left endpoint             */
	float       b = 25.0;   /* Right endpoint            */
	int         n = 1024;  /* Number of trapezoids      */
	float       h;         /* Trapezoid base length     */
	float       local_a;   /* Left endpoint my process  */
	float       local_b;   /* Right endpoint my process */
	int         local_n;   /* Number of trapezoids for  */
	/* my calculation            */
	float       integral;  /* Integral over my interval */
	float       total;     /* Total integral            */
	int         source;    /* Process sending integral  */
	int         dest = 0;  /* All messages go to 0      */
	int         tag = 0;

	h = (b-a)/n;    /* h is the same for all processes */
	local_a = a;
	local_b = b;
	integral = Trap(local_a, local_b, n, h);

	printf("With n = %d trapezoids, our estimatei of\n", n);
	printf("the integral from %f to %f = %f\n", a, b, integral);

	return 0;
} /*  main  */


float Trap(
  float  local_a   /* in */,
  float  local_b   /* in */,
  int    local_n   /* in */,
  float  h         /* in */) {

	float integral;   /* Store result in integral  */
	float x;
	int i;

	float f(float x); /* function we're integrating */

	integral = (f(local_a) + f(local_b))/2.0;
	x = local_a;
	for (i = 1; i <= local_n-1; i++) {
		x = x + h;
		integral = integral + f(x);
	}
	integral = integral*h;
	return integral;
} /*  Trap  */


float f(float x) {
	float return_val;
	/* Calculate f(x). */
	/* Store calculation in return_val. */
	return_val = x*x;
	return return_val;
} /* f */


