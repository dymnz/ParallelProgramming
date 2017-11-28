#include "RNN.h"
#include <stdio.h>
#include <stdlib.h>

void RNN_init(RNN_t *RNN_storage) {
	int input_vector_len = RNN_storage->input_vector_len;
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

	RNN_storage->internal_state_matrix = matrix_create(0, 0);
}

void RNN_destroy(RNN_t *RNN_storage) {
	matrix_free(RNN_storage->input_weight_matrix);
	matrix_free(RNN_storage->output_weight_matrix);
	matrix_free(RNN_storage->internal_weight_matrix);
	matrix_free(RNN_storage->internal_state_matrix);
	free(RNN_storage);
}

void RNN_forward_propagation(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,	// TxI
    Matrix_t *output_matrix	// TxO
) {
	int i_dim = RNN_storage->input_vector_len;
	int o_dim = RNN_storage->output_vector_len;
	int t_dim = input_matrix->m;
	int h_dim = RNN_storage->hidden_layer_vector_len;

	matrix_resize(
	    RNN_storage->internal_state_matrix,
	    t_dim,
	    RNN_storage->hidden_layer_vector_len);

	math_t **X = input_matrix->data;
	math_t **O = output_matrix->data;

	math_t **S = RNN_storage->internal_state_matrix->data;	// TxH
	math_t **U = RNN_storage->input_weight_matrix->data;	// IxH
	math_t **V = RNN_storage->output_weight_matrix->data;	// HxO
	math_t **W = RNN_storage->internal_weight_matrix->data;	// HxH

	int m, n, r, t;

	for (m = 0; m < t_dim; ++m) {
		for (n = 0; n < h_dim; ++n) {
			S[m][n] = 0.0;
		}
	}

	// S[0] = X[0]*U as S[-1] = [0]
	// 1xH =  1xI*IxH
	for (n = 0; n < h_dim; ++n) {
		for (r = 0; r < i_dim; ++r) {
			S[0][n] += X[0][r] * U[r][n];
		}
	}
	for (n = 0; n < h_dim; ++n) {
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
				S[t][n] += S[t - 1][r] * W[r][n];
			}
			S[t][n] = internal_squash_func(S[t][n]);
		}
	}


	// O[t] = S[t] * V
	// 1xO = 1xH * HxO
	math_t *temp_vector = (math_t *) malloc(o_dim * sizeof(math_t));
	for (t = 0; t < t_dim; ++t) {
		for (n = 0; n < o_dim; ++n) {
			temp_vector[n] = 0.0;
			for (r = 0; r < h_dim; ++r) {
				// O[t] = S[t] * V
				temp_vector[n] += S[t][r] * V[r][n];
			}
		}
		stable_softmax(temp_vector, O[t], o_dim);
	}

	free(temp_vector);
}

// Cross entropy loss
math_t RNN_loss_calculation(
    RNN_t *RNN_storage,
    Matrix_t *predicted_output_matrix,	// TxO
    Matrix_t *expected_output_matrix	// TxO
) {
	math_t total_loss = 0.0, log_term = 0.0;

	int t_dim = predicted_output_matrix->m;
	int o_dim = RNN_storage->output_vector_len;

	int t, o;
	for (t = 0; t < t_dim; ++t) {
		// expected_output_matrix is an one-hot vector
		for (o = 0; o < o_dim; ++o) {
			if (expected_output_matrix->data[t][o] == 1) {
				log_term = log(predicted_output_matrix->data[t][o]);
				break;
			}
		}
		total_loss += -1.0 / t_dim * log_term;
	}

	return total_loss;
}

void RNN_BPTT(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,				// TxI
    Matrix_t *predicted_output_matrix,	// TxO
    Matrix_t *expected_output_matrix,	// TxO
    Matrix_t *input_weight_gradient,	// dLdU IxH
    Matrix_t *output_weight_gradient,	// dLdV HxO
    Matrix_t *internel_weight_gradient	// dLdW HxH
) {
	int t_dim = input_matrix->m;
	int i_dim = RNN_storage->input_vector_len;
	int o_dim = RNN_storage->output_vector_len;
	int h_dim = RNN_storage->hidden_layer_vector_len;

	math_t **delta_o
	    = create_2d(t_dim, o_dim);
	math_t *delta_t
	    = (math_t *) malloc(h_dim * sizeof(math_t));

	math_t **S = RNN_storage->internal_state_matrix->data;	// TxH
	math_t **V = RNN_storage->output_weight_matrix->data;	// HxO
	math_t **W = RNN_storage->internal_weight_matrix->data;	// HxH

	math_t **dLdU = input_weight_gradient->data;	// IxH
	math_t **dLdV = output_weight_gradient->data;	// HxO
	math_t **dLdW = internel_weight_gradient->data; // HxH

	math_t **X = input_matrix->data;
	math_t **O = predicted_output_matrix->data;
	math_t **Y = expected_output_matrix->data;

	clear_2d(dLdU, i_dim, h_dim);
	clear_2d(dLdV, h_dim, o_dim);
	clear_2d(dLdW, h_dim, h_dim);

	int t, o, bptt_t, m, n;

	/*
		Construct delta_o = (o - y) = (y' - y)
	 */
	// Copy O
	for (t = 0; t < t_dim; ++t) {
		for (o = 0; o < o_dim; ++o) {
			delta_o[t][o] = O[t][o];
		}
	}

	// For each time step, subtract the one-hot indexc
	for (t = 0; t < t_dim; ++t) {
		for (o = 0; o < o_dim; ++o) {
			if (Y[t][o] == 1) {
				delta_o[t][o] -= 1;
				break;
			}
		}
	}

	/*
		BPTT
	 */
	// math_t **dLdU = input_weight_gradient->data;		// IxH
	// math_t **dLdV = output_weight_gradient->data;	// HxO
	// math_t **dLdW = internel_weight_gradient->data;	// HxH
	for (t = t_dim - 1; t >= 0; --t) {
		// Update dLdV += outer(delta_o[t], S[t]')
		for (m = 0; m < h_dim; ++m) {
			for (n = 0; n < o_dim; ++n) {
				// Outer product
				dLdV[m][n] += S[t][m] * delta_o[t][n];
			}
		}

		// Update delta_t = V' dot delta_o[t] * (1 - S[t]^2)
		// HxO * Ox1 .* Hx1
		for (m = 0; m < h_dim; ++m) {
			delta_t[m] = 0;
			for (n = 0; n < o_dim; ++n) {
				delta_t[m] += V[m][n] * delta_o[t][n];
			}
			delta_t[m] *= (1 - S[t][m] * S[t][m]);
		}

		// BPTT: From t to 0, S[-1] = [0]
		for (bptt_t = t; bptt_t >= 0; bptt_t--)
		{
			// Update dLdW += outer(delta_t, S[t-1])
			if (bptt_t - 1 >= 0) {
				for (m = 0; m < h_dim; ++m) {
					for (n = 0; n < h_dim; ++n) {
						dLdW[m][n] += S[bptt_t - 1][m] * delta_t[n];
					}
				}
			}

			// Update dLdU[x[bptt_step]] += delta_t
			for (m = 0; m < i_dim; ++m) {
				if (X[bptt_t][m] == 1) {
					for (n = 0; n < h_dim; ++n) {
						dLdU[m][n] += delta_t[n];
					}
					break;
				}
			}

			// Update delta_t = W' dot delta_o[t] * (1 - S[t-1]^2)
			// HxO * Ox1 .* Hx1
			for (m = 0; m < h_dim; ++m) {
				delta_t[m] = 0;
				for (n = 0; n < h_dim; ++n) {
					delta_t[m] += delta_t[n] * W[n][m];
				}
				if (bptt_t - 1 >= 0)
					delta_t[m] *= (1 - S[bptt_t - 1][m] * S[bptt_t - 1][m]);
			}
		}

	}

}

void RNN_SGD(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,
    Matrix_t *expected_output_matrix,
    Matrix_t *predicted_output_matrix,
    Matrix_t *input_weight_gradient,
    Matrix_t *output_weight_gradient,
    Matrix_t *internel_weight_gradient,
    math_t learning_rate
) {

	math_t **U = RNN_storage->input_weight_matrix->data;	// IxH
	math_t **V = RNN_storage->output_weight_matrix->data;	// HxO
	math_t **W = RNN_storage->internal_weight_matrix->data;	// HxH

	math_t **dLdU = input_weight_gradient->data;	// IxH
	math_t **dLdV = output_weight_gradient->data;	// HxO
	math_t **dLdW = internel_weight_gradient->data; // HxH

	int i_dim = RNN_storage->input_vector_len;
	int o_dim = RNN_storage->output_vector_len;
	int h_dim = RNN_storage->hidden_layer_vector_len;

	RNN_forward_propagation(
	    RNN_storage,
	    input_matrix,
	    predicted_output_matrix
	);

	RNN_BPTT(
	    RNN_storage,
	    input_matrix,				// TxI
	    predicted_output_matrix,	// TxO
	    expected_output_matrix,		// TxO
	    input_weight_gradient,		// dLdU IxH
	    output_weight_gradient,		// dLdV HxO
	    internel_weight_gradient	// dLdW HxH
	);


	int m, n;

	// Update U
	for (m = 0; m < i_dim; ++m)
		for (n = 0; n < h_dim; ++n)
			U[m][n] -= learning_rate * dLdU[m][n];

	// Update V
	for (m = 0; m < h_dim; ++m)
		for (n = 0; n < o_dim; ++n)
			V[m][n] -= learning_rate * dLdV[m][n];

	// Update W
	for (m = 0; m < h_dim; ++m)
		for (n = 0; n < h_dim; ++n)
			W[m][n] -= learning_rate * dLdW[m][n];
}


void RNN_train(
    RNN_t *RNN_storage,
    Matrix_t *input_matrix,
    Matrix_t *expected_output_matrix,
    Matrix_t *predicted_output_matrix,
    Matrix_t *input_weight_gradient,
    Matrix_t *output_weight_gradient,
    Matrix_t *internel_weight_gradient,
    math_t initial_learning_rate,
    int max_epoch,
    int print_loss_interval,
    int batch_size
) {
	int t_dim = input_matrix->m;

	int e, t;
	math_t loss;

	int data_batch_index;
	Matrix_t *batch_matrix
	    = (Matrix_t *) malloc(sizeof(Matrix_t));
	batch_matrix->m = batch_size;
	batch_matrix->n = RNN_storage->input_vector_len;

	math_t learning_rate = initial_learning_rate;

	for (e = 0; e < max_epoch; ++e) {
		if (e % print_loss_interval == 0) {
			loss = RNN_loss_calculation(
			           RNN_storage,
			           predicted_output_matrix,
			           expected_output_matrix
			       );
			printf("loss at epoch: %5d = %10lf\n", e, loss);
		}

		for (t = 0; t < t_dim; ) {

			/*
				Splicing the TxI input matrix to fit the batch_size
			*/
			data_batch_index = t;
			batch_matrix->data = &(input_matrix->data[data_batch_index]);

			if (t + batch_size - 1 < t_dim)
				batch_matrix->m = batch_size;
			else
				batch_matrix->m = t_dim - t;

			//printf("data_batch_index: %3d  [%3d]\n", data_batch_index, batch_matrix->m);
			//matrix_print(batch_matrix);

			RNN_SGD(
			    RNN_storage,
			    input_matrix,
			    expected_output_matrix,
			    predicted_output_matrix,
			    input_weight_gradient,
			    output_weight_gradient,
			    internel_weight_gradient,
			    learning_rate
			);

			t += batch_matrix->m;
		}

	}
}

math_t internal_squash_func(math_t value) {
	return tanh(value);
}