/*
	Balanced search space division/Proper/Distance cache/Partial distance compare/
	Copy when swapping
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//#define VERBOSE
//#define DEBUG
#define PRINT_STATUS
#define ENABLE_2OPT_COUNTER
//#define KEEP_DIST_LIST    // Save the calculated distance,
// requires a lot of RAM

#define THREAD_COUNT 16
#define SECONDS_TO_WAIT 10
#define SECONDS_BUFFER 0

typedef double dist_type;

struct City {
	int index;
	int x;
	int y;
};

struct Thread_Param {
	int rank;
	int start_depth;
	int skip_depth;
	int max_depth;
};

void print_route();
void *read_coord(void *fp);
void *read_route(void *fp);
inline dist_type distance(dist_type x1, dist_type y1, dist_type x2, dist_type y2);

void parallel_2opt();
void *parallel_2opt_job(void *param);

dist_type get_city_distance(int index_1, int index_2);
dist_type get_total_route_distance(int *);
dist_type get_partial_route_distance(int *, int, int);
inline dist_type get_swapped_total_route_distance(
    int *route_index_list, int start, int end);
inline dist_type get_swapped_partial_route_distance(
    int *route_index_list, int start, int end);

void two_opt(int start, int end);

int  available_threads = THREAD_COUNT;
int		num_city;
dist_type 	default_distance;

/* Arrays */
int  *route_index_list;
struct City	*city_list;

#ifdef KEEP_DIST_LIST
dist_type	*dist_list;
#endif

// If cache_route_distance == 0, update it; If not, use that value
// This value is change if and only if route_index_list is changed.
dist_type cache_route_distance = 0;

pthread_rwlock_t	route_list_rwlock;

/* Time control*/
int go_flag = 1;
time_t start_time;
pthread_rwlock_t	go_flag_rwlock;

#ifdef ENABLE_2OPT_COUNTER
int *opt_counter_list;
int swap_counter = 0;
int race_cond_counter = 0;
double total_swap_length = 0;
double total_reduced_distance = 0;
#endif

// Casting coord(int) to coord(double) so they don't overflow while computing
inline dist_type distance(dist_type x1, dist_type y1, dist_type x2, dist_type y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

// A single 2-opt swap
void two_opt(int start, int end) {
#ifdef VERBOSE
	printf("two_opt: %3d : %3d\n", start, end);
#endif

	// Do not process the node at the start and the end
	if (start == 0 || end == num_city) {
		printf("Please don't do this\n");
		exit(42);
	}

	// Modifiy the route to check if the new route is shorter
#ifdef DEBUG
	pthread_rwlock_rdlock(&route_list_rwlock);
	printf("Swap route from index: %3d %3d\n", start, end);
	printf("Before swap:\n");
	print_route();
	pthread_rwlock_unlock(&route_list_rwlock);
#endif

	int i;

	/*
	Calculate original distance
	*/
	pthread_rwlock_rdlock(&route_list_rwlock);

	// This line is inside rdlock to protect cache_route_distance from being
	// overwritten.
	dist_type partial_original_distance
	    = get_partial_route_distance(route_index_list, start, end);

	// Find the distance of the new route
	dist_type partial_new_distance
	    = get_swapped_partial_route_distance(route_index_list, start, end);

	pthread_rwlock_unlock(&route_list_rwlock);



	/*
	Change to new route if the new route is shorter.
	Race condition here. A better route may be overwriten.
	*/
	if (partial_new_distance < partial_original_distance) {
		int *new_route_list = (int *) malloc((num_city + 1) * sizeof(int));

		pthread_rwlock_rdlock(&route_list_rwlock);
		// Copy
		memcpy(new_route_list,
		       route_index_list,
		       (num_city + 1) * sizeof(int));
		for (i = 0; i < end - start + 1; ++i) {
			new_route_list[start + i] = route_index_list[end - i];
		}
		pthread_rwlock_unlock(&route_list_rwlock);

		dist_type new_distance = get_swapped_total_route_distance(route_index_list, start, end);

		pthread_rwlock_wrlock(&route_list_rwlock);
		// Check if the route is really shorter to avoid race condition
		if (new_distance >=  cache_route_distance) {
#ifdef ENABLE_2OPT_COUNTER
			++race_cond_counter;
#endif
			pthread_rwlock_unlock(&route_list_rwlock);
			free(new_route_list);
			return;
		}

#ifdef ENABLE_2OPT_COUNTER
		++swap_counter;
		total_swap_length += end - start + 1;
		total_reduced_distance += cache_route_distance - new_distance;
#endif

		free(route_index_list);
		route_index_list = new_route_list;
		cache_route_distance = new_distance; // Whoever changed the route, update the distance

		pthread_rwlock_unlock(&route_list_rwlock);
	}

#ifdef DEBUG
	pthread_rwlock_rdlock(&route_list_rwlock);
	printf("After swap:\n");
	print_route();
	pthread_rwlock_unlock(&route_list_rwlock);
#endif

}

/*
	Search for depth: [start end)
*/
void *parallel_2opt_job(void *param) {
	struct Thread_Param *thread_param = (struct Thread_Param *) param;
	int i, m;

	// i: Depth control
	// m: Loop control
	do {
		for (i = thread_param->start_depth;
		        go_flag && i < thread_param->max_depth;
		        i += thread_param->skip_depth)
		{
			for (m = 1; go_flag && m < num_city - i; ++m) {
#ifdef ENABLE_2OPT_COUNTER
				++opt_counter_list[thread_param->rank];
#endif
				two_opt(m, m + i);
			}
		}
	} while (go_flag);

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
	pthread_t *two_opt_thread_list =
	    (pthread_t *) malloc(available_threads * sizeof(pthread_t));

#ifdef ENABLE_2OPT_COUNTER
	opt_counter_list = (int *) malloc(available_threads * sizeof(pthread_t));
#endif
	int max_depth = num_city - 1;

	// In case there's too many thread available
	int threads_to_use = available_threads;
	if (max_depth < available_threads)
		threads_to_use = max_depth;

	printf("Using %3d threads\n", threads_to_use);

	for (i = 0; i < threads_to_use; ++i) {
		struct Thread_Param *thread_param =
		    (struct Thread_Param *) malloc(sizeof(struct Thread_Param));

		thread_param->rank = i;
		thread_param->start_depth = i + 1;
		thread_param->skip_depth = threads_to_use;
		thread_param->max_depth = max_depth;

#ifdef VERBOSE
		printf("New thread: %5d:%5d\n", thread_param->start_depth, thread_param->skip_depth);
#endif

		pthread_create(&two_opt_thread_list[i],
		               NULL, parallel_2opt_job, (void *)thread_param);
	}

	// Wait for the time up
	while (time(NULL) < start_time + SECONDS_TO_WAIT - SECONDS_BUFFER) {
#ifdef PRINT_STATUS
		if ( time(NULL) - start_time > 0 && (time(NULL) - start_time) % 30 == 0 ) {
			printf("Distance @ %2lu:%02lu = %lf\n",
			       (unsigned long)(time(NULL) - start_time) / 60,
			       (unsigned long)(time(NULL) - start_time) % 60,
			       cache_route_distance);
		}
#endif
		sleep(1);
	}
	// Change go_flag to 0
	go_flag = 0;

	for (i = 0; i < threads_to_use; ++i)
		pthread_join(two_opt_thread_list[i], NULL);
}

/*
	Distance array is a serialized 2D-array with -1 as default value.
	The distance is calculated when needed.
*/
dist_type get_city_distance(int index_1, int index_2) {

#ifdef KEEP_DIST_LIST
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
#else
	return distance(
	           city_list[index_1].x,
	           city_list[index_1].y,
	           city_list[index_2].x,
	           city_list[index_2].y);
#endif
}

/*
	Calculate the total route distance when switching start and end index
*/
inline dist_type get_swapped_total_route_distance(
    int *route_index_list,
    int start, int end)
{
	dist_type distance_sum = cache_route_distance;

	// Remove old
	distance_sum -=
	    get_city_distance(
	        route_index_list[start - 1],
	        route_index_list[start]);
	distance_sum -=
	    get_city_distance(
	        route_index_list[end],
	        route_index_list[end + 1]);

	// Add new
	distance_sum +=
	    get_city_distance(
	        route_index_list[start - 1],
	        route_index_list[end]);
	distance_sum +=
	    get_city_distance(
	        route_index_list[start],
	        route_index_list[end + 1]);
	return distance_sum;
}

/*
	Calculate the total route distance
*/
inline dist_type get_total_route_distance(int *route_index_list) {
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
	Calculate the distance after swapping index start and end.
*/
inline dist_type get_swapped_partial_route_distance(
    int *route_index_list,
    int start, int end)
{
	dist_type distance_sum;

	distance_sum = get_city_distance(route_index_list[start - 1], route_index_list[end]);
	distance_sum += get_city_distance(route_index_list[start], route_index_list[end + 1]);

	return distance_sum;
}

/*
	Calculate the distance between index start and end.
*/
inline dist_type get_partial_route_distance(
    int *route_index_list,
    int start, int end)
{
	dist_type distance_sum;

	distance_sum = get_city_distance(route_index_list[start - 1], route_index_list[start]);
	distance_sum += get_city_distance(route_index_list[end], route_index_list[end + 1]);

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

	fprintf(fpOutput, "%f\n", get_total_route_distance(route_index_list));
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

	// Use the thread_num in argument if available
	if (argc > 4)
		available_threads = atoi(argv[4]);
	else
		available_threads = THREAD_COUNT;

	printf("Working on %s\n", argv[1]);

	// Read number of city
	fscanf(fpCoord, "%d", &num_city);


#ifdef KEEP_DIST_LIST
	int i;
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
#endif



	// Read city coord and default route
	pthread_create(&fp_thread_list[0], NULL, read_coord, (void*)fpCoord);
	pthread_create(&fp_thread_list[1], NULL, read_route, (void*)fpRoute);
	pthread_join(fp_thread_list[0], NULL);
	pthread_join(fp_thread_list[1], NULL);

	// Init route_list_rwlock and go_flag_rwlock
	pthread_rwlock_init(&route_list_rwlock, NULL);
	pthread_rwlock_init(&go_flag_rwlock, NULL);

	// Init the cache_route_distance
	cache_route_distance = get_total_route_distance(route_index_list);

#ifdef VERBOSE
	printf("Original route:\n");
	print_route();
	printf("Original route distance: %lf\n", get_total_route_distance(route_index_list));
#endif

	printf("Balanced search space division/Proper/Distance cache/Partial distance compare:\n");
	parallel_2opt();

#ifdef VERBOSE
	print_route();
#endif

	printf("Final route distance: %lf\n", get_total_route_distance(route_index_list));

	// Write to file
	write_route(fpOutput);

#ifdef ENABLE_2OPT_COUNTER
	int i;
	int total_opt_count = 0;
	for (i = 0; i < available_threads; ++i)
		total_opt_count += opt_counter_list[i];

	printf("call: %7d swap: %7d %%: %.2f race: %3d %%: %.2f avg_swap_length: %.2lf avg_dist_dec: %.2lf\n",
	       total_opt_count,
	       swap_counter,
	       swap_counter * 100.0f / total_opt_count,
	       race_cond_counter,
	       race_cond_counter * 100.0f / total_opt_count,
	       total_swap_length / swap_counter,
	       total_reduced_distance / swap_counter);
#endif


	/* Cleanup */
	fclose(fpOutput);
	fclose(fpCoord);
	fclose(fpRoute);

	free(city_list);
	free(route_index_list);

	pthread_rwlock_destroy(&route_list_rwlock);
	pthread_rwlock_destroy(&go_flag_rwlock);

	return 0;
}
