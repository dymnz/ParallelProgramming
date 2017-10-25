/*
	OpenMP version of ans_2opt_sp11.c
	* Balanced search space division/Proper/Distance cache/
	  Partial distance compare/Copy when swapping
	* OpenMP Distributed range/Free to swap/Contention alert
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

//#define VERBOSE
//#define DEBUG
#define PRINT_STATUS
#define ENABLE_2OPT_COUNTER
//#define KEEP_DIST_LIST	// Save the calculated distance,
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

dist_type get_city_distance(int index_1, int index_2);
dist_type get_total_route_distance(int *);
dist_type get_partial_route_distance(int *, int, int);
inline dist_type get_swapped_total_route_distance(
    int *route_index_list, int start, int end);
inline dist_type get_swapped_partial_route_distance(
    int *route_index_list, int start, int end);

void two_opt_swap(int start, int end);
dist_type two_opt_check(int start, int end);

int available_threads = THREAD_COUNT;
int num_city;
dist_type default_distance;

/* Arrays */
int  *route_index_list;
struct City	*city_list;


#ifdef KEEP_DIST_LIST
dist_type *dist_list;
#endif

// If cache_route_distance == 0, update it; If not, use that value
// This value is change if and only if route_index_list is changed.
dist_type cache_route_distance = 0;


/* Time control*/
int go_flag = 1;
time_t start_time;

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

/*
	Execute 2opt swap from index at start to end
*/
void two_opt_swap(int start, int end) {
#ifdef VERBOSE
	printf("two_opt swap: %3d : %3d\n", start, end);
#endif

	int i, temp;

	#pragma omp parallel for private(temp)
	for (i = 0; i < end - start; ++i) {
		temp = route_index_list[end - i];
		route_index_list[end - i] = route_index_list[start + i];
		route_index_list[start + i] = temp;
	}
}

/*
	Return the difference in distance after swapping index at start and end.
	distance_reduced = original - swapped
*/
dist_type two_opt_check(int start, int end) {
#ifdef VERBOSE
	printf("two_opt check: %3d : %3d\n", start, end);
#endif

#ifdef ENABLE_2OPT_COUNTER
	opt_counter_list[omp_get_thread_num()]++;
#endif

	dist_type partial_original_distance
	    = get_partial_route_distance(route_index_list, start, end);

	// Find the distance of the new route
	dist_type partial_new_distance
	    = get_swapped_partial_route_distance(route_index_list, start, end);

	return partial_original_distance - partial_new_distance;
}

/*
	Parallel version of complete 2-opt.
*/
void parallel_2opt() {
	int start, depth;
	int best_thread_num;

	int max_depth = num_city - 1;

	// In case there's too many thread available
	int threads_to_use = available_threads;
	if (max_depth < available_threads)
		threads_to_use = max_depth;

	omp_set_num_threads(threads_to_use);

	printf("Using %3d threads\n", threads_to_use);

#ifdef ENABLE_2OPT_COUNTER
	opt_counter_list = (int *) malloc(available_threads * sizeof(int));
#endif

	int omp_chunk_size = num_city / available_threads;

	// Keep track of the ending position of each thread to avoid contention
	int *thread_process_end_list = (int *)
	                              malloc((available_threads - 1) * sizeof(int));

	while (go_flag) {
		#pragma omp parallel for collapse(2) \
		shared(go_flag, route_index_list) \
		schedule(static, omp_chunk_size)
		for (depth = 1; depth < max_depth; ++depth) {
			for (start = 1; start < num_city - depth; ) {

				// Keep the list fresh
				#pragma omp flush(route_index_list)

				/*
					Check contention status
					Contention happens when the this thread's end
					goes over the chunk limit and affects the start of the next thread.

					If contention happens, the next thread moves to the next un-contented
					pivot.
				*/
				#pragma omp flush(thread_process_end_list)
				if (omp_get_thread_num() > 1 &&
					thread_process_end_list[omp_get_thread_num() - 1] + 1 >= start)
					start = thread_process_end_list[omp_get_thread_num() - 1] + 2;

				if (two_opt_check(start, start + depth) > 0)
					two_opt_swap(start, start + depth);

				#pragma omp single
				check_time();

				if (!go_flag) break;
			}
			if (!go_flag) break;
		}
	}
}

void check_time() {
	// Check time
	go_flag = time(NULL) <
	          start_time + SECONDS_TO_WAIT - SECONDS_BUFFER;

#ifdef PRINT_STATUS
	if (
	    (time(NULL) - start_time) % 30 == 0 &&
	    (time(NULL) - start_time) > 0)
	{
		printf("Distance @ %2lu:%02lu = %lf\n",
		       (unsigned long)(time(NULL) - start_time) / 60,
		       (unsigned long)(time(NULL) - start_time) % 60,
		       cache_route_distance);
	}
#endif
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
	read_route(fpRoute);
	read_coord(fpCoord);



	// Init the cache_route_distance
	cache_route_distance = get_total_route_distance(route_index_list);

#ifdef DEBUG
	printf("Original route:\n");
	print_route();
	printf("Original route distance: %lf\n", get_total_route_distance(route_index_list));
#endif

	printf("Balanced search space division/Proper/Distance cache/Partial distance compare:\n");
	parallel_2opt();

#ifdef DEBUG
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

	return 0;
}
