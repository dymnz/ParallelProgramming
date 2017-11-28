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
	math_t data_in[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	Matrix_t *input_matrix, *output_matrix;
	matrix_prepare(&input_matrix, T, I, data_in);
	output_matrix = matrix_create(T, I);

	printf("-------------input_matrix\n");
	matrix_print(input_matrix);

	RNN_t *RNN_storage
	    = (RNN_t *) malloc(sizeof(RNN_t));

	RNN_storage->input_vector_len = I;

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

void RNN_Loss_test() {
	int T = 4, I = 4;
	int H = 3, O = 4;

	// hell
	math_t data_in[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0
	};

	// ello
	math_t data_out[] = {
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	Matrix_t *input_matrix, *expected_output_matrix, *predicted_output_matrix;
	matrix_prepare(&input_matrix, T, I, data_in);
	matrix_prepare(&expected_output_matrix, T, O, data_out);
	predicted_output_matrix = matrix_create(T, O);

	printf("-------------input_matrix\n");
	matrix_print(input_matrix);
	printf("-------------expected_output_matrix\n");
	matrix_print(expected_output_matrix);

	RNN_t *RNN_storage
	    = (RNN_t *) malloc(sizeof(RNN_t));

	RNN_storage->input_vector_len = I;

	RNN_storage->output_vector_len = O;
	RNN_storage->hidden_layer_vector_len = H;
	RNN_storage->bptt_truncate_len = 4;

	RNN_init(RNN_storage);
	RNN_forward_propagation(RNN_storage, input_matrix, predicted_output_matrix);

	printf("-------------predicted_output_matrix\n");
	matrix_print(predicted_output_matrix);

	math_t total_loss = RNN_loss_calculation(
		RNN_storage, 
		predicted_output_matrix, 
		expected_output_matrix);
	printf("total_loss: %lf\n", total_loss);
	printf("expected_loss: %lf\n", log(I));

	RNN_destroy(RNN_storage);
	matrix_free(input_matrix);
	matrix_free(expected_output_matrix);
	matrix_free(predicted_output_matrix);
}

void RNN_BPTT_test() {
	int T = 5, I = 4;
	int H = 4, O = 4;

	// hell
	math_t data_in[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// ello
	math_t data_out[] = {
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
		1, 0, 0, 0
	};

	Matrix_t *input_matrix, *expected_output_matrix, *predicted_output_matrix;
	matrix_prepare(&input_matrix, T, I, data_in);
	matrix_prepare(&expected_output_matrix, T, I, data_out);
	predicted_output_matrix = matrix_create(T, I);

	printf("-------------input_matrix\n");
	matrix_print(input_matrix);
	printf("-------------expected_output_matrix\n");
	matrix_print(expected_output_matrix);

	RNN_t *RNN_storage
	    = (RNN_t *) malloc(sizeof(RNN_t));

	RNN_storage->input_vector_len = I;

	RNN_storage->output_vector_len = I;
	RNN_storage->hidden_layer_vector_len = H;
	RNN_storage->bptt_truncate_len = 4;

	RNN_init(RNN_storage);
	RNN_forward_propagation(RNN_storage, input_matrix, predicted_output_matrix);

	printf("-------------predicted_output_matrix\n");
	matrix_print(predicted_output_matrix);

	math_t total_loss = RNN_loss_calculation(
		RNN_storage, 
		predicted_output_matrix, 
		expected_output_matrix);
	printf("total_loss: %lf\n", total_loss);
	printf("expected_loss: %lf\n", log(I));

	Matrix_t *input_weight_gradient;
	Matrix_t *output_weight_gradient;
	Matrix_t *internel_weight_gradient;
	input_weight_gradient = matrix_create(I, H);
	output_weight_gradient = matrix_create(H, O);
	internel_weight_gradient = matrix_create(H, H);

	printf("RNN_BPTT\n");

	RNN_BPTT(
		RNN_storage, 
		input_matrix, 
		predicted_output_matrix,
		expected_output_matrix,
		input_weight_gradient,
		output_weight_gradient,
		internel_weight_gradient
	);
	printf("-------------input_weight_gradient\n");
	matrix_print(input_weight_gradient);
	printf("-------------output_weight_gradient\n");
	matrix_print(output_weight_gradient);
	printf("-------------internel_weight_gradient\n");
	matrix_print(internel_weight_gradient);

	RNN_destroy(RNN_storage);
	matrix_free(input_matrix);
	matrix_free(expected_output_matrix);
	matrix_free(predicted_output_matrix);
	matrix_free(input_weight_gradient);
	matrix_free(output_weight_gradient);
	matrix_free(internel_weight_gradient);
}

void RNN_Train_test() {
	int T = 5, I = 4;
	int H = 4, O = 4;

	math_t initial_learning_rate = 0.005;
    int max_epoch = 1000;
    int print_loss_interval = 10;
    int batch_size = 1;

	// hell
	math_t data_in[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// ello
	math_t data_out[] = {
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
		1, 0, 0, 0
	};

	Matrix_t *input_matrix, *expected_output_matrix, *predicted_output_matrix;
	matrix_prepare(&input_matrix, T, I, data_in);
	matrix_prepare(&expected_output_matrix, T, I, data_out);
	predicted_output_matrix = matrix_create(T, I);

	printf("-------------input_matrix\n");
	matrix_print(input_matrix);
	printf("-------------expected_output_matrix\n");
	matrix_print(expected_output_matrix);

	RNN_t *RNN_storage
	    = (RNN_t *) malloc(sizeof(RNN_t));
	RNN_storage->input_vector_len = I;
	RNN_storage->output_vector_len = I;
	RNN_storage->hidden_layer_vector_len = H;
	RNN_storage->bptt_truncate_len = 4;
	RNN_init(RNN_storage);

	Matrix_t *input_weight_gradient;
	Matrix_t *output_weight_gradient;
	Matrix_t *internel_weight_gradient;
	input_weight_gradient = matrix_create(I, H);
	output_weight_gradient = matrix_create(H, O);
	internel_weight_gradient = matrix_create(H, H);

	RNN_train(
	    RNN_storage,
	    input_matrix,
	    expected_output_matrix,
	    predicted_output_matrix,
	    input_weight_gradient,
	    output_weight_gradient,
	    internel_weight_gradient,
	    initial_learning_rate,
	    max_epoch,
	    print_loss_interval,
	    batch_size
	);

	printf("Prediction:\n");
	RNN_Predict(
		RNN_storage,
		input_matrix,
		predicted_output_matrix
	);

	RNN_destroy(RNN_storage);
	matrix_free(input_matrix);
	matrix_free(expected_output_matrix);
	matrix_free(predicted_output_matrix);
	matrix_free(input_weight_gradient);
	matrix_free(output_weight_gradient);
	matrix_free(internel_weight_gradient);
}

int main()
{

	//RNN_FP_test();	
	//RNN_Loss_test();
	//RNN_BPTT_test();
	RNN_Train_test();
	return 0;
}
