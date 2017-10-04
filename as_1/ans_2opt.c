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
void find_dist_test(struct City *city_list);
inline float distance(int x1, int y1, int x2, int y2);

int num_city;
float default_dist;

inline float distance(int x1, int y1, int x2, int y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

void find_dist_test(struct City *city_list) {
	int num_edge = num_city  * (num_city - 1) / 2;	// C(N, 2)
	float *dist_list = (float *) malloc(num_edge * sizeof(float));

	int count = 0, i, j;
	for (i = 0; i < num_city - 1; ++i) {
		for (j = i + 1; j < num_city; ++j) {
			printf("%3d: %3d-%3d\n", count, i, j);
			dist_list[count] =
			    distance(
			        city_list[i].x,
			        city_list[i].y,
			        city_list[j].x,
			        city_list[j].y);
			++count;
		}
	}
}

void *read_route(void *fp) {
	FILE *fpRoute = (FILE *) fp;
	int *route_index_list = (int *) malloc(num_city * sizeof(int));
	
	fscanf(fpRoute, "%f", &default_dist);
	int index = 0;
	while (fscanf(fpRoute, "%d", &(route_index_list[index])) != EOF);

	return (void *) route_index_list;
}

void *read_coord(void *fp) {
	FILE *fpCoord = (FILE *) fp;
	struct City *city_list;
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

	return (void *) city_list;
}

int main(int argc, char const *argv[])
{
	int i;
	int *route_index_list;
	float *dist_list;
	struct City *city_list;
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

	// Read city coord and default route
	pthread_create(&fp_thread_list[0], NULL, read_coord, (void*)fpCoord);  
	pthread_create(&fp_thread_list[1], NULL, read_route, (void*)fpRoute); 
	pthread_join(fp_thread_list[0], (void **) &city_list); 
	pthread_join(fp_thread_list[1], (void **) &route_index_list); 

	// 
	find_dist_test(city_list);

	free(city_list);
	free(route_index_list);
}