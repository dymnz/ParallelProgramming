#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

//#define VERBOSE

#define THREAD_COUNT 8
//#define DEBUG

typedef double dist_type;

struct City {
	int index;
	int x;
	int y;
};

struct Thread_Param {
	int start;
	int end;
	int max_depth;
};

void print_route();
void *read_coord(void *fp);
void *read_route(void *fp);
inline dist_type distance(int x1, int y1, int x2, int y2);

void parallel_2opt();
void *parallel_2opt_job(void *param);

dist_type get_city_distance(int index_1, int index_2);
dist_type get_route_distance();

void two_opt(int start, int end);

int		num_city;
dist_type 	default_distance;

int		*route_index_list;
dist_type	*dist_list;
struct City	*city_list;

pthread_rwlock_t	rwlock;

int opt_counter = 0;
pthread_rwlock_t	counter_rwlock;

inline dist_type distance(int x1, int y1, int x2, int y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

// A signle 2-opt swap
void two_opt(int start, int end) {

#ifdef VERBOSE
	printf("two_opt: %3d : %3d\n", start, end);
#endif

	pthread_rwlock_wrlock(&counter_rwlock);
	++opt_counter;
	pthread_rwlock_unlock(&counter_rwlock);

	// Do not process the node at the start and the end
	if (start == 0 || end == num_city) {
		printf("Please don't do this\n");
		exit(42);
	}

	pthread_rwlock_rdlock(&rwlock);
	dist_type original_distance =
	    get_city_distance(route_index_list[start - 1], route_index_list[start])
	    + get_city_distance(route_index_list[end], route_index_list[end + 1]);
	dist_type swapped_distance =
	    get_city_distance(route_index_list[start - 1], route_index_list[end]);
	+ get_city_distance( route_index_list[start], route_index_list[end + 1]);
	pthread_rwlock_unlock(&rwlock);

	// Modifiy the route if the new route is shorter
	if (original_distance > swapped_distance) {
#ifdef DEBUG
		printf("Swap route from index: %3d %3d\n", start, end);
		printf("Before swap:\n");
		print_route();
#endif

		int i, temp_i;
		int swap_count = (end - start + 1) / 2;

		pthread_rwlock_wrlock(&rwlock);
		for (i = 0; i < swap_count; ++i) {
			temp_i = route_index_list[start + i];
			route_index_list[start + i] = route_index_list[end - i];
			route_index_list[end - i] = temp_i;
		}
		pthread_rwlock_unlock(&rwlock);

#ifdef DEBUG
		printf("After swap:\n");
		print_route();
#endif
	}
}

void *parallel_2opt_job(void *param) {
	struct Thread_Param *thread_param = (struct Thread_Param *) param;
	
	//printf("From: %3ld:%3ld:%3ld\n", (long)1, thread_param->max_depth, num_city - thread_param->max_depth);

	long i;
	for (i = 1; i < num_city - thread_param->max_depth + 1; ++i) {
		two_opt(i, i + thread_param->max_depth - 1);
	}

	free(thread_param);

	return NULL;
}

/*
	Parallel version of complete 2-opt.

*/
void parallel_2opt() {
	int i, j;
	pthread_t two_opt_thread_list[THREAD_COUNT];

	int max_depth = num_city - 1; // Change this to control run time

	for (i = 0; i < max_depth; ) {
		for (j = 0; j < THREAD_COUNT; ++j) {

			struct Thread_Param *thread_param =
			    (struct Thread_Param *) malloc(sizeof(struct Thread_Param));

			thread_param->max_depth = (i + 2);

			pthread_create(&two_opt_thread_list[j],
			               NULL, parallel_2opt_job, (void *)thread_param);
			++i;
		}

		for (j = 0; j < THREAD_COUNT; ++j)
			pthread_join(two_opt_thread_list[j], NULL);
	}

}

/*
	Distance array is a serialized 2D-array with -1 as default value.
	The distance is calculated when needed.
*/
dist_type get_city_distance(int index_1, int index_2) {
	int array_index;

	// The last node is equal to the first node
	if (index_2 == num_city)
		index_2 = 0;

	if (index_1 > index_2) {
		array_index = num_city * (index_2)
		              - (index_2 + 1) * index_2 / 2
		              + (index_1 - index_2 - 1);
	} else {
		array_index = num_city * (index_1)
		              - (index_1 + 1) * index_1 / 2
		              + (index_2 - index_1 - 1);
	}

	if (dist_list[array_index] == -1) {
		dist_list[array_index] =
		    distance(
		        city_list[index_1].x,
		        city_list[index_1].y,
		        city_list[index_2].x,
		        city_list[index_2].y);
	}

#ifdef DEBUG
	printf("Distance: %2d: %2d:%2d = %3f\n", array_index, index_1, index_2, dist_list[array_index]);
#endif

	return dist_list[array_index];
}


/*
	Calculate the total route distance
*/
dist_type get_route_distance() {
	int i;
	int index_1, index_2;
	dist_type distance_sum = 0.0;
	for (i = 0; i < num_city; ++i) {
		index_1 = route_index_list[i];
		index_2 = route_index_list[i + 1];
		distance_sum += get_city_distance(index_1, index_2);
	}
	return distance_sum;
}

/*
	Print every node, including dummy last node
*/
void print_route() {
	int i;
	for (i = 0; i < num_city + 1; ++i) {
		printf("%3d ", route_index_list[i] + 1);
	}
	printf("\n");
}

/*
	Print every node to file, not including dummy last node
*/
void write_route(FILE *fpOutput) {
	int i;

	fprintf(fpOutput, "%f\n", get_route_distance());
	for (i = 0; i < num_city; ++i) {
		fprintf(fpOutput, "%d\n", route_index_list[i] + 1);
	}
}

/*
	Read original distance and route from file
*/
void *read_route(void *fp) {
	FILE *fpRoute = (FILE *) fp;
	route_index_list = (int *) malloc((num_city + 1) * sizeof(int));

	fscanf(fpRoute, "%lf", &default_distance);
	int index = 0, temp_i;
	while (fscanf(fpRoute, "%d", &temp_i) != EOF) {
		route_index_list[index++] = temp_i - 1;
	}

	// Append the first node to the last
	route_index_list[num_city] = route_index_list[0];

	return NULL;
}

/*
	Read city coords from file. The # of city line is processed in main()
*/
void *read_coord(void *fp) {
	FILE *fpCoord = (FILE *) fp;
	struct City temp_city = {
		.index = -1,
		.x = -1,
		.y = -1,
	};

	// Init storage
	city_list = (struct City *) malloc(num_city * sizeof(struct City));

	int index = 0;
	while (fscanf(fpCoord, "%d %d %d",
	              &(temp_city.index),
	              &(temp_city.x),
	              &(temp_city.y)) != EOF)
	{
		--temp_city.index;
		city_list[index] = temp_city;

		++index;
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	int i;
	pthread_t fp_thread_list[2];

	if (argc < 3) {
		printf("not enough input\n");
		exit(69);
	}

	// Open file pointers
	FILE *fpCoord, *fpRoute, *fpOutput;
	fpCoord = fopen (argv[1], "r");
	if (fpCoord == NULL) exit(1);
	fpRoute = fopen (argv[2], "r");
	if (fpRoute == NULL) exit(2);
	fpOutput = fopen (argv[3], "w");
	if (fpOutput == NULL) exit(3);

	// Read number of city
	fscanf(fpCoord, "%d", &num_city);
	int num_edge = num_city  * (num_city - 1) / 2;	// C(N, 2)

	// Distances are reset to -1
	dist_list = (dist_type *) malloc(num_edge * sizeof(dist_type));
	for (i = 0; i < num_edge; ++i) {
		dist_list[i] = -1;
	}

	// Read city coord and default route
	pthread_create(&fp_thread_list[0], NULL, read_coord, (void*)fpCoord);
	pthread_create(&fp_thread_list[1], NULL, read_route, (void*)fpRoute);
	pthread_join(fp_thread_list[0], NULL);
	pthread_join(fp_thread_list[1], NULL);

	// Init rwlock
	pthread_rwlock_init(&rwlock, NULL);
	pthread_rwlock_init(&counter_rwlock, NULL);

#ifdef VERBOSE
	printf("Original route:\n");
	print_route();
	printf("Original route distance: %lf\n", get_route_distance());
#endif

	printf("Parallel test algo_2:\n");
	parallel_2opt();
#ifdef VERBOSE	
	print_route();
#endif

	printf("Final route distance: %lf\n", get_route_distance());

	// Write to file
	write_route(fpOutput);

	printf("2opt_call: %d\n", opt_counter);

	/* Cleanup */
	fclose(fpOutput);
	fclose(fpCoord);
	fclose(fpRoute);

	free(city_list);
	free(route_index_list);
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlock_destroy(&counter_rwlock);

	return 0;
}