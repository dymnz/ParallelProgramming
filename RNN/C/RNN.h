#pragma once
#include "common_math.h"

typedef struct
{
	int input_vector_len;
	int input_window_len;

	int output_vector_len;

	int hidden_layer_vector_len;
	int bptt_truncate_len;

	Matrix_t *internal_state_matrix;	// S TxH

	Matrix_t *input_weight_matrix;		// U IxH
	Matrix_t *output_weight_matrix;		// V HxO
	Matrix_t *internal_weight_matrix;	// W HxH

} RNN_t;

void RNN_init(RNN_t *RNN_storage);
void RNN_forward_propagation(
	RNN_t *RNN_storage,
	Matrix_t *input_vector,
	Matrix_t *output_vector
);

math_t internal_squash_func(math_t value);
