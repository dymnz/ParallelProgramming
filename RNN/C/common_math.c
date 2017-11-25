#pragma once
#include <stdlib.h>
#include "common_math.h"

math_t uniform_random_with_seed(
    math_t lower_bound,
    math_t upper_bound,
    unsigned int *seedp
){
	return (rand_r(seedp) / RAND_MAX) *
	       (upper_bound - lower_bound) +
	       lower_bound;
}
