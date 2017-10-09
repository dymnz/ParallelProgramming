/*
	Parllization test 3
	Serial job parallalized
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


//#define VERBOSE
//#define DEBUG
//#define ENABLE_2OPT_COUNTER
#define THREAD_COUNT 16
#define SECONDS_TO_WAIT 10

typedef double dist_type;

struct City {
	int index;
	int x;
	int y;
};

struct Thread_Param {
	int start_depth;
	int end_depth;
};

void print_route();
void *read_coord(void *fp);
void *read_route(void *fp);
inline dist_type distance(dist_type x1, dist_type y1, dist_type x2, dist_type y2);

void parallel_2opt();
void *parallel_2opt_job(void *param);

dist_type get_city_distance(int index_1, int index_2);
dist_type get_route_distance();

void two_opt(int start, int end);

int		num_city;
dist_type 	default_distance;

/* Arrays */
int		*route_index_list;
dist_type	*dist_list;
struct City	*city_list;

pthread_rwlock_t	route_list_rwlock;

/* Time control*/
int go_flag = 1;
time_t start_time;
pthread_rwlock_t	go_flag_rwlock;

#ifdef ENABLE_2OPT_COUNTER
int opt_counter = 0;
pthread_rwlock_t	counter_rwlock;
#endif

// Casting coord(int) to coord(double) so they don't overflow while computing
inline dist_type distance(dist_type x1, dist_type y1, dist_type x2, dist_type y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

// A signle 2-opt swap
void two_opt(int start, int end) {

#ifdef VERBOSE
	printf("two_opt: %3d : %3d\n", start, end);
#endif

#ifdef ENABLE_2OPT_COUNTER
	pthread_rwlock_wrlock(&counter_rwlock);
	++opt_counter;
	pthread_rwlock_unlock(&counter_rwlock);
#endif

	// Do not process the node at the start and the end
	if (start == 0 || end == num_city) {
		printf("Please don't do this\n");
		exit(42);
	}

	pthread_rwlock_rdlock(&route_list_rwlock);
	dist_type original_distance =
	    get_city_distance(route_index_list[start - 1], route_index_list[start])
	    + get_city_distance(route_index_list[end], route_index_list[end + 1]);
	dist_type swapped_distance =
	    get_city_distance(route_index_list[start - 1], route_index_list[end]);
	+ get_city_distance( route_index_list[start], route_index_list[end + 1]);
	pthread_rwlock_unlock(&route_list_rwlock);

	// Modifiy the route if the new route is shorter
	if (original_distance > swapped_distance) {
#ifdef DEBUG
		printf("Swap route from index: %3d %3d\n", start, end);
		printf("Before swap:\n");
		print_route();
#endif		

		int i;
		int route_segment_length = end - start + 1;
		int *new_route_segment = (int *) malloc(route_segment_length * sizeof(int));

		// Prepare new route segment		
		pthread_rwlock_rdlock(&route_list_rwlock);
		for (i = 0; i < route_segment_length; ++i) {
			new_route_segment[i] = route_index_list[end - i];
		}
		pthread_rwlock_unlock(&route_list_rwlock);

		// Change the route
		pthread_rwlock_wrlock(&route_list_rwlock);
		memcpy(route_index_list + start, 
			new_route_segment, 
			route_segment_length * sizeof(int));
		pthread_rwlock_unlock(&route_list_rwlock);

		free(new_route_segment);

#ifdef DEBUG
		printf("After swap:\n");
		print_route();
#endif
	}
}

/*
	Search for depth: [start end)
	TODO: Check go_flag
*/
void *parallel_2opt_job(void *param) {
	struct Thread_Param *thread_param = (struct Thread_Param *) param;
	int i, m;

	// i: Depth control
	// m: Loop control
	do {
		for (i = thread_param->start_depth; i < thread_param->end_depth; ++i) {
			for (m = 1; m < num_city - i; ++m) {
				two_opt(m, m + i);        
			}
      if (time(NULL) > start_time + SECONDS_TO_WAIT)
				  break;     
		} 
	} while (time(NULL) < start_time + SECONDS_TO_WAIT);
  
	free(thread_param);

	return NULL; 
}


/*
	Parallel version of complete 2-opt.
	Split the search space into segments by dividing chunck of depth

	Alternative: Balance the workload by comb dividing
*/
void parallel_2opt() {
	int i;
	pthread_t two_opt_thread_list[THREAD_COUNT];

	int max_depth = num_city - 1;

	// In case there's too many thread available
	int threads_to_use = THREAD_COUNT;
	if (max_depth < THREAD_COUNT)
		threads_to_use = max_depth;

	int depth_segment_size = max_depth / threads_to_use;

	for (i = 0; i < threads_to_use; ++i) {
		struct Thread_Param *thread_param =
		    (struct Thread_Param *) malloc(sizeof(struct Thread_Param));

		thread_param->start_depth = (depth_segment_size * i) + 1;
		thread_param->end_depth = (depth_segment_size * (i + 1)) + 1;
    printf("New thread: %5d:%5d\n", thread_param->start_depth, thread_param->end_depth);
		pthread_create(&two_opt_thread_list[i],
		               NULL, parallel_2opt_job, (void *)thread_param);
	}

	// Wait for the time up
	while (time(NULL) - start_time < SECONDS_TO_WAIT);

	// Change go_flag to 0
	pthread_rwlock_wrlock(&go_flag_rwlock);
	go_flag = 0;
	pthread_rwlock_unlock(&go_flag_rwlock);

	for (i = 0; i < threads_to_use; ++i)
		pthread_join(two_opt_thread_list[i], NULL);
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
	if (route_index_list == NULL) {
		printf("route_index_list malloc() error\n");
		exit(56);
	}	

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
	if (city_list == NULL) {
		printf("city_list malloc() error\n");
		exit(34);
	}

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

	if (argc < 4) {
		printf("not enough input\n");
		exit(69);
	}

	// Get program start time
	start_time = time(NULL);
	go_flag = 1;

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
	if (dist_list == NULL) {
		printf("dist_list malloc() error\n");
		exit(12);
	}

	for (i = 0; i < num_edge; ++i) {
		dist_list[i] = -1;
	}

	// Read city coord and default route
	pthread_create(&fp_thread_list[0], NULL, read_coord, (void*)fpCoord);
	pthread_create(&fp_thread_list[1], NULL, read_route, (void*)fpRoute);
	pthread_join(fp_thread_list[0], NULL);
	pthread_join(fp_thread_list[1], NULL);

	// Init route_list_rwlock and go_flag_rwlock
	pthread_rwlock_init(&route_list_rwlock, NULL);
	pthread_rwlock_init(&go_flag_rwlock, NULL);


#ifdef ENABLE_2OPT_COUNTER
	pthread_rwlock_init(&counter_rwlock, NULL);
#endif

#ifdef VERBOSE
	printf("Original route:\n");
	print_route();
	printf("Original route distance: %lf\n", get_route_distance());
#endif

	printf("Naive search space division:\n");
	parallel_2opt();

#ifdef VERBOSE
	print_route();
#endif

	printf("Final route distance: %lf\n", get_route_distance());

	// Write to file
	write_route(fpOutput);

#ifdef ENABLE_2OPT_COUNTER
	printf("2opt_call: %d\n", opt_counter);
#endif

	/* Cleanup */
	fclose(fpOutput);
	fclose(fpCoord);
	fclose(fpRoute);

	free(city_list);
	free(route_index_list);

	pthread_rwlock_destroy(&route_list_rwlock);
	pthread_rwlock_destroy(&go_flag_rwlock);

#ifdef ENABLE_2OPT_COUNTER
	pthread_rwlock_destroy(&counter_rwlock);
#endif

	return 0;
}
