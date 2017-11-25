#include <stdio.h>
#include <stdlib.h>
#include "common_math.h"

void uniform_random_with_seed_test() {
	int round = 1000000;

	unsigned int seed = 10;
	math_t lower_bound = 101;
	math_t upper_bound = 1100;

	int array_size = (int)(upper_bound - lower_bound) + 1;
	int *array = (int *) malloc(array_size * sizeof(math_t));

	int i;
	for (i = 0; i < round; ++i) {
		math_t rand_num =
		    uniform_random_with_seed(lower_bound, upper_bound, &seed);
		int index = (int)rand_num - (int)lower_bound;
		++array[index];
	}

	for (i = 0; i < array_size; ++i) {
		printf("%5d: %5d\n", i + (int)lower_bound, array[i]);
	}
}

void matrix_mult_test() {
	math_t data_a[] = {1, 2, 3, 3, 2, 1};
	math_t data_b[] = {3, 5, 7, 9, 8, 6, 4, 2};

	Matrix_t *matrix_a, *matrix_b;

	matrix_a = matrix_create(3, 2);
	if (!matrix_a) {
		printf("Error");
		return;
	}
	matrix_b = matrix_create(2, 4);
	if (!matrix_b) {
		printf("Error");
		return;
	}

	int m, n;
	for (m = 0; m < matrix_a->m; ++m)
		for (n = 0; n < matrix_a->n; ++n)
			matrix_a->data[m][n] = data_a[m*matrix_a->n + n];
	for (m = 0; m < matrix_b->m; ++m)
		for (n = 0; n < matrix_b->n; ++n)
			matrix_b->data[m][n] = data_b[m*matrix_b->n + n];

	printf("-------------A\n");
	matrix_print(matrix_a);
	printf("-------------B\n");
	matrix_print(matrix_b);

	Matrix_t *matrix_c = matrix_mult(matrix_a, matrix_b);
	if (!matrix_c) {
		printf("Error");
		return;
	}

	printf("-------------C\n");
	matrix_print(matrix_c);


}

int main()
{
	//uniform_random_with_seed_test();
	//matrix_mult_test();
	return 0;
}


