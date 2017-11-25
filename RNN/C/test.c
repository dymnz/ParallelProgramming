#include <stdio.h>
#include <stdlib.h>
#include "common_math.h"
#include "RNN.h"

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
void matrix_prepare(Matrix_t **m_ptr, int m, int n, math_t *data) {

	*m_ptr = matrix_create(m, n);
	Matrix_t *matrix = *m_ptr;
	if (!matrix)
		exit(69);

	for (m = 0; m < matrix->m; ++m)
		for (n = 0; n < matrix->n; ++n)
			matrix->data[m][n] = data[m * matrix->n + n];
}

void RNN_FP_test() {
	int T = 4, I = 4;
	int H = 3;
	math_t data[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	Matrix_t *input_matrix, *output_matrix;
	matrix_prepare(&input_matrix, T, I, data);
	output_matrix = matrix_create(T, I);

	printf("-------------input_matrix\n");
	matrix_print(input_matrix);

	RNN_t *RNN_storage
	    = (RNN_t *) malloc(sizeof(RNN_t));

	RNN_storage->input_vector_len = I;
	RNN_storage->input_window_len = T;

	RNN_storage->output_vector_len = I;
	RNN_storage->hidden_layer_vector_len = H;
	RNN_storage->bptt_truncate_len = 4;

	RNN_init(RNN_storage);

	printf("-------------input_weight_matrix\n");
	matrix_print(RNN_storage->input_weight_matrix);
	printf("-------------output_weight_matrix\n");
	matrix_print(RNN_storage->output_weight_matrix);
	printf("-------------internal_weight_matrix\n");
	matrix_print(RNN_storage->internal_weight_matrix);

	RNN_forward_propagation(RNN_storage, input_matrix, output_matrix);
	printf("-------------internal_state_matrix\n");
	matrix_print(RNN_storage->internal_state_matrix);
	printf("-------------output_matrix\n");
	matrix_print(output_matrix);
}
int main()
{

	RNN_FP_test();
	return 0;
}
