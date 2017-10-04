#include <stdio.h>
#include <phread.h>

int num_city;

int main(int argc, char const *argv[])
{
	int i;
	struct City *city_list;
	pthread_t fp_thread_list[3];		

	// Open file pointers
	FILE *fpCoord, *fpRoute, *fpOutput;
	fpCoord = fopen (argv[1], "r");
	fpRoute = fopen (argv[2], "r");
	fpOutput = fopen (argv[3], "w");

	// Read city coord
	pthread_create(&thread_handles[0], NULL, 
		read_coord, (void*)fpCoord);  

	pthread_join(thread_handles[thread], (void **) &city_list); 
	printf("===========================\n");
	for (i = 0; i < num_city; ++i) 
		printf("%3d %3d %3d\n", city_list[i].index - 1, city_list[i].x, city_list[i].y);

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
	fscanf(fpCoord, "%d", &num_city);
	city_list = (struct City*) malloc(sizeof(struct City) * num_city);
	int index = 0;
	while (fscanf(fpCoord, "%d %d %d",
	              &(temp_city.index),
	              &(temp_city.x),
	              &(temp_city.y)) != EOF)
	{
		#ifdef DEBUG
		printf("%3d %3d %3d\n", temp_city.index - 1, temp_city.x, temp_city.y);
		#endif
		--temp_city.index;
		city_list[index] = temp_city;

		++index;
	}

	return (void *) city_list;
}