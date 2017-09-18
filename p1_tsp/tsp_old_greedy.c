// Compile with -lm flag b/c of math.h: gcc ./tsp.c -o tsp -lm
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define VERBOSE
#define DEBUG

#define BOOL int
#define TRUE 1
#define FALSE 0

struct City {
	int index;
	int x;
	int y;
	BOOL visited;
};



int* bubble_sort(double array[], int length) {
	int i, j, i_temp;
	double d_temp;
	int* index_list = (int*) malloc(sizeof(int) * length);

	for (i = 0; i < length; ++i)
		index_list[i] = i;

	for (i = 0; i < length-1; ++i) {
		for (j = 0; j < length-i-1; ++j) {
			if (array[j+1] < array[j]) {
				d_temp = array[j];
				array[j] = array[j+1];
				array[j+1] = d_temp;

				i_temp = index_list[j];
				index_list[j] = index_list[j+1];
				index_list[j+1] = i_temp;
			}
		}
	}

	return index_list;
}


double distance(int x1, int y1, int x2, int y2) {
	return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
}

double* find_distance(struct City* city_list, int num_city, struct City* current_city) {
	// Remember to free the malloc
	double* dist_list = (double *) malloc(sizeof(double) * num_city);
	int i;
	for (i = 0; i < num_city; ++i) {
		dist_list[i] = distance(
			current_city->x, 
			current_city->y,
			city_list[i].x, 
			city_list[i].y);
	}

	return dist_list;
}

double TSP(struct City* city_list, int num_city) {
	// Starting from the first city
	int i;
	int* answer_list = (int *) malloc(sizeof(int) * num_city);
	int answer_index = 0;
	int next_city_index = 0;
	int next_city_dist_index = 0;
	double dist_sum = 0.0;

	struct City* current_city = &(city_list[0]);
	double* dist_list = find_distance(city_list, num_city, current_city);

	while (TRUE) {
	current_city->visited = TRUE;
	answer_list[answer_index] = current_city->index;
	++answer_index;

	#ifdef VERBOSE
		printf("%3d | Travel to %d\n", answer_index, current_city->index);
	#endif	

	// Find the list of distance
	dist_list = find_distance(city_list, num_city, current_city);

	// End not reached
	if (answer_index < num_city) {
		// Sort to increasing order
		int* index_list = bubble_sort(dist_list, num_city);

		#ifdef DEBUG
		for (i = 0; i < num_city; ++i) {
			printf("%7.3f", dist_list[i]);
		}
		printf("\n");
		for (i = 0; i < num_city; ++i) {
			printf("%7d", index_list[i]);
		}
		printf("\n");
		#endif

		for (i = 0; i < num_city; ++i) {
		// If the city is visited, continue
			if (city_list[index_list[i]].visited == TRUE)
				continue;
			else {
				next_city_index = index_list[i];
				next_city_dist_index = i;
				break;
			}
		}

		dist_sum += dist_list[next_city_dist_index];
		current_city = &(city_list[next_city_index]);
		answer_list[answer_index] = next_city_index+1;
		free(index_list);
		#ifdef VERBOSE
		printf("next_city_index:%3d next_city_dist_index: %3d\n", next_city_index, next_city_dist_index);
		printf("Current dist %lf\n", dist_sum);
		#endif

	} else {
		dist_sum += dist_list[0];

		break;
	}


	
	free(dist_list);
	
	}
	for (i = 0; i < num_city; ++i)
		printf("%d\n", answer_list[i]);
	return dist_sum;
}


int main(int argc, char const *argv[])
{
	FILE* pInput;
	struct City* city_list;
	struct City temp_city;
	int num_city;
	int i;


  	pInput = fopen (argv[1],"r");	
  	fscanf(pInput, "%d", &num_city);

  	city_list = (struct City*) malloc(sizeof(struct City) * num_city);
  	
  	int index = 0;
  	while(fscanf(pInput, "%d %d %d", 
  		&(temp_city.index), 
  		&(temp_city.x), 
  		&(temp_city.y)) != EOF) 
  	{
  		printf("%3d %3d %3d\n", temp_city.index, temp_city.x, temp_city.y);
  		city_list[index].index = temp_city.index;
  		city_list[index].x = temp_city.x;
  		city_list[index].y = temp_city.y; 		
  		city_list[index].visited = FALSE;

  		if (++index > num_city) 
  			printf("WTF");
  	}

  	double dist = TSP(city_list, num_city);
  	printf("%lf\n", dist);

	return 0;
}
