#pragma once
#include "common_math.h"

typedef struct
{
	int input_vector_len;

	int output_vector_len;

	int hidden_layer_vector_len;
	int bptt_truncate_len;

	Matrix_t *internal_state_matrix;	// S TxH

	Matrix_t *input_weight_matrix;		// U IxH
	Matrix_t *output_weight_matrix;		// V HxO
	Matrix_t *internal_weight_matrix;	// W HxH
} RNN_t;

void RNN_init(RNN_t *RNN_storage);
void RNN_destroy(RNN_t *RNN_storage);

void RNN_forward_propagation(
	RNN_t *RNN_storage,
	Matrix_t *input_matrix,
	Matrix_t *output_matrix
);
math_t RNN_loss_calculation(
    RNN_t *RNN_storage,
    Matrix_t *predicted_output_matrix,	// TxO
    Matrix_t *expected_output_matrix	// TxO
);
void RNN_BPTT(
	RNN_t *RNN_storage,
	Matrix_t *input_matrix,
	Matrix_t *predicted_output_matrix,
	Matrix_t *expected_output_matrix,
	Matrix_t *input_weight_gradient,	// dLdU
	Matrix_t *output_weight_gradient,	// dLdV
	Matrix_t *internel_weight_gradient	// dLdW
);


math_t internal_squash_func(math_t value);
