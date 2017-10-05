#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG

struct City {
	int index;
	int x;
	int y;
};

void *read_coord(void *fp);
void *read_route(void *fp);
float get_city_distance(int index_1, int index_2);
float get_route_distance();
inline float distance(int x1, int y1, int x2, int y2);

int num_city;
float default_distance;

int *route_index_list;
float *dist_list;
struct City *city_list;	

inline float distance(int x1, int y1, int x2, int y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

/*
	Distance array is a serialized 2D-array with -1 as default value.
	The distance is calculated when needed.
*/
float get_city_distance(int index_1, int index_2) {
	int array_index;

	if (index_1 > index_2) {
		array_index = num_city * (index_2 - 1) 
						- index_2 * (index_2 - 1) / 2
						+ (index_1 - index_2 - 1);
	} else {
		array_index = num_city * (index_1 - 1) 
						- index_1 * (index_1 - 1) / 2
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

	return dist_list[array_index];
}

float get_route_distance() {
	int i;
	int index_1, index_2; 	
	float distance_sum = 0.0;
	for (i = 0; i < num_city - 1; ++i) {
		index_1 = route_index_list[i];
		index_2 = route_index_list[i+1];
		distance_sum += get_city_distance(index_1, index_2);
	}

	return distance_sum;
}

void *read_route(void *fp) {
	FILE *fpRoute = (FILE *) fp;
	route_index_list = (int *) malloc(num_city * sizeof(int));
	
	fscanf(fpRoute, "%f", &default_distance);
	int index = 0, temp_i;
	while (fscanf(fpRoute, "%d", &temp_i) != EOF) {
		route_index_list[index++] = temp_i - 1;
	}

	return NULL;
}

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
	dist_list = (float *) malloc(num_edge * sizeof(float));
	for (i = 0; i < num_edge; ++i) {
    	dist_list[i] = -1;
	}

	// Read city coord and default route
	pthread_create(&fp_thread_list[0], NULL, read_coord, (void*)fpCoord);  
	pthread_create(&fp_thread_list[1], NULL, read_route, (void*)fpRoute); 
	pthread_join(fp_thread_list[0], NULL); 
	pthread_join(fp_thread_list[1], NULL); 
	
	printf("Original route distance: %f\n", get_route_distance());

	free(city_list);
	free(route_index_list);

	return 0;
}