#pragma once
#include "common_math.h"

typedef struct {
	int input_vector_len;

	int output_vector_len;

	int hidden_layer_vector_len;
	int bptt_truncate_len;

	Matrix_t *internal_state_matrix;	// S TxH

	Matrix_t *input_weight_matrix;		// U IxH
	Matrix_t *output_weight_matrix;		// V HxO
	Matrix_t *internal_weight_matrix;	// W HxH
} RNN_t;

typedef struct {
	Matrix_t **input_matrix_list;
	Matrix_t **output_matrix_list;
	int num_matrix;
	int max_matrix_len;
} TrainSet_t;

void TrainSet_init(TrainSet_t *train_set, int num_matrix);
void TrainSet_destroy(TrainSet_t *train_set);

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

void RNN_SGD(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,
    Matrix_t *expected_output_matrix,
    Matrix_t *predicted_output_matrix,
    Matrix_t *input_weight_gradient,
    Matrix_t *output_weight_gradient,
    Matrix_t *internel_weight_gradient,
    math_t learning_rate    
);

void RNN_train(
    RNN_t *RNN_storage,
    TrainSet_t *train_set,
    Matrix_t *predicted_output_matrix,
    Matrix_t *input_weight_gradient,
    Matrix_t *output_weight_gradient,
    Matrix_t *internel_weight_gradient,
    math_t initial_learning_rate,
    int max_epoch,
    int print_loss_interval
);

void RNN_Predict(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,    
    Matrix_t *predicted_output_matrix
);

math_t internal_squash_func(math_t value);
