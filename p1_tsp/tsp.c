// Compile with -lm flag b/c of math.h: gcc ./tsp.c -o tsp -lm
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define VERBOSE
//#define DEBUG

#define BOOL int
#define TRUE 1
#define FALSE 0

#define DIRECTION int
#define LEFT 1
#define RIGHT 0

struct City {
	int index;
	int x;
	int y;
	int edge_count;
	struct City *left;
	struct City *right;
};

struct CityNode {
	int slot[2];
};

struct Edge {
	int index_1;
	int index_2;
	double dist;
};

FILE *pInput, *pOutput;

void swap(struct Edge *v1, struct Edge *v2) {
	struct Edge v_temp = *v1;
	*v1 = *v2;
	*v2 = v_temp;
}

void bubble_sort(struct Edge *edge_list, int length) {
	int i, j, i_temp;

	for (i = 0; i < length - 1; ++i) {
		for (j = 0; j < length - i - 1; ++j) {
			if (edge_list[j + 1].dist < edge_list[j].dist) {
				swap(&(edge_list[j + 1]), &(edge_list[j]));
			}
		}
	}
}

double distance(int x1, int y1, int x2, int y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

struct City* root(struct City *city) {
	struct City *ptr = city;

	while (ptr->right != NULL)
		ptr = ptr->right;

	return ptr;
}

void reverse_link(struct City *city, DIRECTION dir) {
	struct City *tracer = city;
	struct City *temp;
	if (dir == RIGHT)
		while (tracer != NULL) {
			temp = tracer->right;
			tracer->right = tracer->left;
			tracer->left = temp;
			tracer = tracer->left;
		}
	else
		while (tracer != NULL) {
			temp = tracer->left;
			tracer->left = tracer->right;
			tracer->right = temp;
			tracer = tracer->right;
		}
}

void TSP(struct City *city_list, int num_city) {
	int i, j;
	int num_edge = num_city  * (num_city - 1) / 2;	// C(10, 2)
	struct Edge *edge_list = (struct Edge*) malloc(sizeof(struct Edge) * num_edge);
	struct Edge **sel_edge_list = (struct Edge**) malloc(sizeof(struct Edge*) * num_city);

	// Find the edge of each pair
	double *dist_list;
	int count = 0;
	for (i = 0; i < num_city - 1; ++i) {
		for (j = i + 1; j < num_city; ++j) {
			edge_list[count].index_1 = i;
			edge_list[count].index_2 = j;
			edge_list[count].dist =
			    distance(
			        city_list[i].x,
			        city_list[i].y,
			        city_list[j].x,
			        city_list[j].y);
			++count;
		}
	}

	// Sort the edge_list
	bubble_sort(edge_list, num_edge);

#ifdef DEBUG
	for (i = 0; i < num_edge; ++i)
		printf("%3d %3d: %3lf\n",
		       edge_list[i].index_1,
		       edge_list[i].index_2,
		       edge_list[i].dist);
#endif

	struct Edge *current_edge;
	struct City *city_1, *city_2, *root_1, *root_2;
	int sel_edge_count = 0;
	int edge_index = 0;

	while (sel_edge_count < num_city) {
		// Grab an edge
		current_edge = &(edge_list[edge_index]);
		++edge_index;

		city_1 = &(city_list[current_edge->index_1]);
		city_2 = &(city_list[current_edge->index_2]);

		// If an degree(vertex) >= 2, continue
		if (city_1->edge_count >= 2 || city_2->edge_count >= 2)
			continue;

		root_1 = root(city_1);
		root_2 = root(city_2);

		// Check for loop: If two vertex have the same root, continue
		// The last edge is allowed to have loop.
		// Any other vertex will have an edge_count of 2,
		// so simply select the ones without
		if (sel_edge_count < num_city - 1 && root_1 == root_2)
			continue;

#ifdef VERBOSE
		printf("--------------------------\n");
		printf("city_1: %3d \t city_2: %3d\n", city_1->index, city_2->index);
		printf("root_1: %3d \t root_2: %3d\n", root_1->index, root_2->index);
#endif

		// Connect the two city_link
		// Here we don't try to find the head or tail of the city
		// and connect them head to tail,
		// but instead determine if the two city is at the head or tail,
		// then connect them in different way.

		if (city_1->right == NULL) {
			// City_2 head to City_1 head
			if (city_2->left != NULL) {
				// Flip City_2 so City_2 head becomes City_2 end
				reverse_link(city_2, LEFT);
			}
			// City_2 end to City_1 head
			city_1->right = city_2;
			city_2->left = city_1;
		} else if (city_2->right == NULL) {
			// City_1 head to City_2 head
			if (city_1->left != NULL) {
				// Flip City_1 so City_1 head becomes City_1 end
				reverse_link(city_1, LEFT);
			}
			// City_1 end to City_2 head
			city_1->left = city_2;
			city_2->right = city_1;
		} else if (city_1->left == NULL && city_2->left == NULL) {
			// City_1 end to City_2 end
			reverse_link(city_1, RIGHT);
			city_1->right = city_2;
			city_2->left = city_1;
		}

		++(city_1->edge_count);
		++(city_2->edge_count);
		sel_edge_list[sel_edge_count] = current_edge;
		++sel_edge_count;

#ifdef VERBOSE
		printf("Edge %3d accepted: %3d %3d\n",
		       sel_edge_count,
		       current_edge->index_1,
		       current_edge->index_2);
#endif
	}

#ifdef DEBUG
	int city_index = 0;
	printf("Good edges:\n");
	for (i = 0; i < num_city; ++i) {
		printf("%3d - %3d\n",
		       sel_edge_list[i]->index_1,
		       sel_edge_list[i]->index_2);
	}
#endif

	// Sum length of edges
	double dist_sum = 0.0;
	for (i = 0; i < num_city; ++i) {
		dist_sum += sel_edge_list[i]->dist;
	}
	printf("%lf\n", dist_sum);
	fprintf(pOutput, "%lf\n", dist_sum);

	// Print from the direction of the smaller neighbor
	struct City* tracer = &(city_list[0]);
	DIRECTION print_dir = RIGHT;
	if (tracer->right->index > tracer->left->index)
		print_dir = LEFT;

	for (i = 0; i < num_city; ++i) {
		printf("%d\n", tracer->index + 1);
		fprintf(pOutput, "%d\n", tracer->index + 1);
		tracer = print_dir == RIGHT ? tracer->right : tracer->left;
	}
	printf("end\n");
	fprintf(pOutput, "end");

	free(sel_edge_list);
	free(edge_list);
}

int main(int argc, char const *argv[])
{

	struct City *city_list;
	struct City temp_city = {
		.index = -1,
		.x = -1,
		.y = -1,
		.edge_count = 0,
		.left = NULL,
		.right = NULL
	};
	int num_city;
	int i;

	pInput = fopen (argv[1], "r");
	pOutput = fopen (argv[2], "w");
	fscanf(pInput, "%d", &num_city);

	city_list = (struct City*) malloc(sizeof(struct City)  * num_city);

	int index = 0;
	while (fscanf(pInput, "%d %d %d",
	              &(temp_city.index),
	              &(temp_city.x),
	              &(temp_city.y)) != EOF)
	{
#ifdef DEBUG
		printf("%3d %3d %3d\n", temp_city.index - 1, temp_city.x, temp_city.y);
#endif
		--temp_city.index;
		city_list[index] = temp_city;

		if (++index > num_city)
			printf("WTF");
	}

	TSP(city_list, num_city);

	free(city_list);

	return 0;
}
