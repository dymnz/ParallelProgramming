#include <stdio.h>
#include <stdlib.h>
#include "common_math.h"

math_t uniform_random_with_seed(
    math_t lower_bound,
    math_t upper_bound,
    unsigned int *seedp
) {
	return ((math_t)rand_r(seedp) / (math_t)RAND_MAX) *
	       (upper_bound - lower_bound + 1) +
	       lower_bound;
}

Matrix_t *matrix_create(int m, int n) {
	Matrix_t *matrix = (Matrix_t *) malloc(sizeof(Matrix_t));
	if(!matrix)
		return NULL;

	matrix->m = m;
	matrix->n = n;

	math_t **row_data = (math_t **) malloc(m * sizeof(math_t *));
	if (!row_data)
		return NULL;

	matrix->data = row_data;

	int i;
	math_t *col_data;
	for (i = 0; i < m; ++i) {
		col_data = (math_t *) malloc(n * sizeof(math_t));
		if (!col_data)
			return NULL;

		matrix->data[i] = col_data;
	}

	return matrix;
}

void matrix_destroy(Matrix_t *matrix) {
	int i;
	for (i = 0; i < matrix->n; ++i)
		free(matrix->data[i]);

	free(matrix->data);
}

void matrix_print(Matrix_t *matrix) {
	int m, n;
	for (m = 0; m < matrix->m; ++m) {
		for (n = 0; n < matrix->n; ++n) {
			printf("%3.3lf   ", matrix->data[m][n]);
		}
		printf("\n");
	}
}

Matrix_t *matrix_mult(Matrix_t *matrix_a, Matrix_t *matrix_b) {
	if (matrix_a->n != matrix_b->m)
		return NULL;

	Matrix_t *matrix_c = matrix_create(matrix_a->m, matrix_b->n);
	if (!matrix_c)
		return NULL;

	int m, n, r, c;
	for (m = 0; m < matrix_a->m; ++m) {
		for (n = 0; n < matrix_b->n; ++n) {
			matrix_c->data[m][n] = 0.0;
			for (c = 0; c < matrix_a->n; ++c) {
				for (r = 0; r < matrix_b->m; ++r)
					matrix_c->data[m][n] += 
						matrix_a->data[m][c] * matrix_b->data[r][n];
			}
		}
	}

	return matrix_c;
}