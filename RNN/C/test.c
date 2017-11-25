#include <stdio.h>
#include <stdlib.h>
#include "common_math.h"

void uniform_random_with_seed_test() {
	int seed = 10;

	math_t lower_bound = 1;
	math_t upper_bound = 100;

	int array_size = (int)(upper_bound - lower_bound) + 1;

	math_t *array = (math_t *) malloc(array_size * sizeof(math_t));

	int i;
	for (i = 0; i < array_size; ++i) {
		math_t rand_num =
		    uniform_random_with_seed(lower_bound, upper_bound, &seed);
		++array[(int) rand_num];
	}

	for(i = 0; i < array_size; ++i) {
		printf("%5d: %5d\n", i, array[i]);
	}
}

int main(int argc, char const *argv[])
{
	uniform_random_with_seed_test();
	return 0;
}


