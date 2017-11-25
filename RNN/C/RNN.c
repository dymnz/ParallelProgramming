#include "RNN.h"
#include <stdio.h>
#include <stdlib.h>

void RNN_init(RNN_t *RNN_storage) {
	int input_vector_len = RNN_storage->input_vector_len;
	int input_window_len = RNN_storage->input_window_len;
	int output_vector_len = RNN_storage->output_vector_len;
	int hidden_layer_vector_len = RNN_storage->hidden_layer_vector_len;

	RNN_storage->input_weight_matrix
	= matrix_create(input_vector_len, hidden_layer_vector_len);
	if (!RNN_storage->input_weight_matrix) {
		printf("input_weight_matrix init error\n");
		exit(69);
	}

	RNN_storage->output_weight_matrix
	= matrix_create(hidden_layer_vector_len, output_vector_len);
	if (!RNN_storage->output_weight_matrix) {
		printf("output_weight_matrix init error\n");
		exit(69);
	}

	RNN_storage->internal_weight_matrix
	= matrix_create(hidden_layer_vector_len, hidden_layer_vector_len);
	if (!RNN_storage->internal_weight_matrix) {
		printf("internal_weight_matrix init error\n");
		exit(69);
	}

	unsigned int seed = 10;
	matrix_random_with_seed(
	    RNN_storage->input_weight_matrix,
	    -sqrt(1 / input_vector_len),
	    sqrt(1 / input_vector_len),
	    &seed);
	matrix_random_with_seed(
	    RNN_storage->output_weight_matrix,
	    -sqrt(1 / hidden_layer_vector_len),
	    sqrt(1 / hidden_layer_vector_len),
	    &seed);
	matrix_random_with_seed(
	    RNN_storage->internal_weight_matrix,
	    -sqrt(1 / hidden_layer_vector_len),
	    sqrt(1 / hidden_layer_vector_len),
	    &seed);

	RNN_storage->internal_state_matrix
	= matrix_create(input_window_len, hidden_layer_vector_len);
}

void RNN_forward_propagation(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,	// TxI
    Matrix_t *output_matrix	// TxO
) {
	math_t **X = input_matrix->data;
	math_t **O = output_matrix->data;

	math_t **S = RNN_storage->internal_state_matrix->data;	// TxH
	math_t **U = RNN_storage->input_weight_matrix->data;	// IxH
	math_t **V = RNN_storage->output_weight_matrix->data;	// HxO
	math_t **W = RNN_storage->internal_weight_matrix->data;	// HxH

	int i_dim = RNN_storage->input_vector_len;
	int o_dim = RNN_storage->output_vector_len;
	int t_dim = RNN_storage->input_window_len;
	int h_dim = RNN_storage->hidden_layer_vector_len;

	int m, n, r, t;

	for (m = 0; m < t_dim; ++m) {
		for (n = 0; n < h_dim; ++n) {
			S[m][n] = 0.0;
		}
	}

	// S[0] = X[0]*U
	// 1xH =  1xI*IxH
	for (n = 0; n < h_dim; ++n) {
		for (r = 0; r < i_dim; ++r) {
			S[0][n] += X[0][r] * U[r][n];
		}
		S[0][n] = internal_squash_func(S[0][n]);
	}

	// S[t] = X[t]*U + S[t-1]*W
	// 1xH =  1xI*IxH + 1xH*HxH
	for (t = 1; t < t_dim; ++t) {
		for (n = 0; n < h_dim; ++n) {
			for (r = 0; r < i_dim; ++r) {
				// S[t] = X[t]*U 
				S[t][n] += X[t][r] * U[r][n]; 
			}
			for (r = 0; r < h_dim; ++r) {
				// S[t] = X[t]*U + S[t-1]*W
				S[t][n] = S[t-1][r] * W[r][n] + S[t][n]; 
			}
			S[t][n] = internal_squash_func(S[t][n]);
		}
	}

	// O[t] = S[t] * V
	// 1xO = 1xH * HxO
	math_t *temp_vector = (math_t *) malloc(o_dim * sizeof(math_t));
	for (t = 0; t < t_dim; ++t) {
		for (n = 0; n < o_dim; ++n) {
			for (r = 0; r < h_dim; ++r) {
				// O[t] = S[t] * V
				temp_vector[n] += S[t][r] * V[r][n]; 
			}			
			softmax(temp_vector, O[t], o_dim);
		}
	}
	free(temp_vector);
}

// Cross entropy loss
math_t RNN_loss_calculation(
    RNN_t *RNN_storage,
    Matrix_t *fp_output_matrix,			// TxO
    Matrix_t *expected_output_matrix	// TxO
) {
	math_t total_loss = 0.0, log_term = 0.0;

	int t_dim = RNN_storage->input_window_len;
	int o_dim = RNN_storage->output_vector_len;

	int t, o;
	for (t = 0; t < t_dim; ++t) {
		log_term = 0.0;
		for (o = 0; o < o_dim; ++o) {			
			log_term += 
				expected_output_matrix->data[t][o] * 
				log(fp_output_matrix->data[t][o]);
		}
		total_loss += -1.0 / t_dim * log_term;
	}

	return total_loss;
}

math_t internal_squash_func(math_t value) {
	return tanh(value);
}