#pragma once

typedef double math_t;
typedef struct Matrix
{
	int m;
	int n;
	math_t **data;
} Matrix_t;


math_t uniform_random_with_seed(
    math_t lower_bound,
    math_t upper_bound,
    unsigned int *seedp
);

Matrix_t *matrix_create(int m, int n);
void matrix_destroy(Matrix_t *matrix);
void matrix_print(Matrix_t *matrix);
Matrix_t *matrix_mult(Matrix_t *matrix_a, Matrix_t *matrix_b);

