#include "B_Motion.h"

/*
	Please refer to "struct Particle" in "B_Motion.h" to check dependency if necessary.
	Functions in B_Motion_init.c don't have to be parallelized.
*/


int main(int argc, char* argv[]){
	// Read argv
	if (argc < 3){
		printf("Usage: ./p2_pth <thread num> <space size>\n");
		exit(0);
	}
	else{
		num_threads = atoi(argv[1]);
		space_size = atoi(argv[2]);
		if (space_size < 1){
			printf("Space size has to be positive number.\n");
			exit(0);
		}
	}
	
	// Initial Environmental Information
	printf("\n=======  Brownian Motion Simulation  =======\n");
	printf("Thread number is %d.\n", num_threads);
	output_file = fopen("answer.txt", "w");
	
	input_data();
	initialize_particles();
	show_environment_info();
	
	// Set up timer
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	/**********  Midterm Exam Start Here  *************/
	
	process();
	
	/**********  Midterm Exam End Here  ***************/
	
	// Calculate execution time
	gettimeofday(&end, NULL);
	double timeInterval = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

	// Get memory usage
	struct rusage r_usage;
	getrusage(RUSAGE_SELF,&r_usage);
	
	// Output performence
	fprintf(output_file, "Memory usage: %ld bytes\n", r_usage.ru_maxrss);
	fprintf(output_file, "Time: %lf sec\n", timeInterval);
	printf("Memory usage: %ld bytes\n", r_usage.ru_maxrss);
	printf("Time: %lf sec\n", timeInterval); 
	
	
	fclose(output_file);
	return 0;
}

void process(){
	
	// Allocate a map to record the movement
	double *x_movement, *y_movement, *z_movement;
	x_movement = (double*)calloc(run_time_step, sizeof(double));
	y_movement = (double*)calloc(run_time_step, sizeof(double));
	z_movement = (double*)calloc(run_time_step, sizeof(double));
	
	// Claim indexes
	long time_step = 0;
	unsigned long long i, j;
	
	
	for (time_step = 0; time_step < run_time_step; time_step++){
		for (i = 0; i < num_particles + 1; i++){
			// Move particles
			move(&(particles[i]));
		}
		for (i = 0; i < num_particles + 1; i++){
			for (j = i; j < num_particles + 1; j++){
				// Calculate the distances between two particles.
				get_distance(&(particles[i]), &(particles[j]));
			}
		}
			
		for (i = 0; i < num_particles + 1; i++){
			for (j = i; j < num_particles + 1; j++){
				// Update speeds for particles if collision occurs.
				collision(&(particles[i]), &(particles[j]));
			}
		}

		// Output result
		fprintf(output_file, "Step: %li   x: %e   y: %e   z: %e\n", time_step, particles[num_particles].x_pos, particles[num_particles].y_pos, particles[num_particles].z_pos);
		printf("\nStep: %li   x: %e   y: %e   z: %e\n", time_step, particles[num_particles].x_pos, particles[num_particles].y_pos, particles[num_particles].z_pos);
		
		// Save the result into map for drawing
		x_movement[time_step] = particles[num_particles].x_pos;
		y_movement[time_step] = particles[num_particles].y_pos;
		z_movement[time_step] = particles[num_particles].z_pos;
	}
	
	// Draw the result
	draw_result(x_movement, y_movement, z_movement);
	
	// Free the space of distace lists
	for (i = 0; i < num_particles + 1; i++){
		free(particles[i].distance_list);
	}
	
	// Free the distace_table
	free(distance_table);
}

void move(struct Particle *particle){
	
	// Update particle position with velocity by 1e-11 second
	particle->x_pos += particle->x_velocity*1e-11;
	particle->y_pos += particle->y_velocity*1e-11;
	particle->z_pos += particle->z_velocity*1e-11;
	
	// Check and update velocity once, when it touches the wall.
	if (!particle->is_near_wall){
		// Left and right wall
		if (particle->x_pos < particle->radius){
			particle->x_velocity *= -1;
			particle->is_near_wall = true;
		}
		else if (particle->x_pos > space_edge - particle->radius){
			particle->x_velocity *= -1;
			particle->is_near_wall = true;
		}
		// Front and back wall
		if (particle->y_pos < particle->radius){
			particle->y_velocity *= -1;
			particle->is_near_wall = true;
		}
		else if (particle->y_pos > space_edge - particle->radius){
			particle->y_velocity *= -1;
			particle->is_near_wall = true;
		}
		// Up and down wall
		if (particle->z_pos < particle->radius){
			particle->z_velocity *= -1;
			particle->is_near_wall = true;
		}
		else if (particle->z_pos > space_edge - particle->radius){
			particle->z_velocity *= -1;
			particle->is_near_wall = true;
		}
	}
	else
		particle->is_near_wall = false;
}

double get_distance(struct Particle *particle, struct Particle *target_particle){
	// Set self-distace = -1
	if (target_particle->index == particle->index){
		particle->distance_list[particle->index] = -1;
		return -1;
	}

	// Calculate the distance between two particles
	double distance;
	distance = sqrt(pow((particle->x_pos-target_particle->x_pos), 2) 
				+ pow((particle->y_pos-target_particle->y_pos), 2) 
				+ pow((particle->z_pos-target_particle->z_pos), 2));
				
	// Update the two distance lists
	particle->distance_list[target_particle->index] = distance;
	set_distance(target_particle, distance, particle->index);
	
	return distance;
}

void set_distance(struct Particle *particle, double value, unsigned long long index){
	particle->distance_list[index] = value;
}

void collision(struct Particle *particle, struct Particle *target_particle){
	// Skip checking itself.
	if (particle->index == target_particle->index)
		return;
	
	// Get distance from the list.
	double distance = particle->distance_list[target_particle->index];
	
	// No collision happen, then return.
	if (distance > particle->radius + target_particle->radius)
		return;
	
	
	/*	A huge part of elastic collision 
		Just need to know that x_pos, y_pos, z_pos are read,
			and x_velocity, y_velocity, z_velocity are updated.
		Ref: https://en.wikipedia.org/wiki/Elastic_collision
	*/
	/******************************************************************/
	double collision_x = target_particle->x_pos - particle->x_pos;
	double collision_y = target_particle->y_pos - particle->y_pos;
	double collision_z = target_particle->z_pos - particle->z_pos;
	
	collision_x /= distance;
	collision_y /= distance;
	collision_z /= distance;
	
	double dot_value = (particle->x_velocity * collision_x + particle->y_velocity * collision_y + particle->z_velocity * collision_z);

	double x_velocity_collision = collision_x * dot_value;
	double y_velocity_collision = collision_y * dot_value;
	double z_velocity_collision = collision_z * dot_value;
	
	double x_velocity_vertical = particle->x_velocity - x_velocity_collision;
	double y_velocity_vertical = particle->y_velocity - y_velocity_collision;
	double z_velocity_vertical = particle->z_velocity - z_velocity_collision;
	
	dot_value = -(target_particle->x_velocity * collision_x + target_particle->y_velocity * collision_y + target_particle->z_velocity * collision_z);
	
	double x_dis_velocity_collision = -collision_x * dot_value;
	double y_dis_velocity_collision = -collision_y * dot_value;
	double z_dis_velocity_collision = -collision_z * dot_value;
	
	double x_dis_velocity_vertical = target_particle->x_velocity - x_dis_velocity_collision;
	double y_dis_velocity_vertical = target_particle->y_velocity - y_dis_velocity_collision;
	double z_dis_velocity_vertical = target_particle->z_velocity - z_dis_velocity_collision;
	
	
	if (particle->mass != target_particle->mass){
		double temp_x_velocity_collision = ((particle->mass - target_particle->mass)/(particle->mass + target_particle->mass))* x_velocity_collision
									+ ((target_particle->mass * target_particle->mass)/(particle->mass + target_particle->mass))* x_dis_velocity_collision;
		double temp_y_velocity_collision = ((particle->mass - target_particle->mass)/(particle->mass + target_particle->mass))* y_velocity_collision
									+ ((target_particle->mass * target_particle->mass)/(particle->mass + target_particle->mass))* y_dis_velocity_collision;							
		double temp_z_velocity_collision = ((particle->mass - target_particle->mass)/(particle->mass + target_particle->mass))* z_velocity_collision
									+ ((target_particle->mass * target_particle->mass)/(particle->mass + target_particle->mass))* z_dis_velocity_collision;
									
		double temp_x_dis_velocity_collision = ((target_particle->mass - particle->mass)/(particle->mass + target_particle->mass))* x_dis_velocity_collision
									+ ((particle->mass * particle->mass)/(particle->mass + target_particle->mass))* x_velocity_collision;
		double temp_y_dis_velocity_collision = ((target_particle->mass - particle->mass)/(particle->mass + target_particle->mass))* y_dis_velocity_collision
									+ ((particle->mass * particle->mass)/(particle->mass + target_particle->mass))* y_velocity_collision;
		double temp_z_dis_velocity_collision = ((target_particle->mass - particle->mass)/(particle->mass + target_particle->mass))* z_dis_velocity_collision
									+ ((particle->mass * particle->mass)/(particle->mass + target_particle->mass))* z_velocity_collision;
		
		x_velocity_collision = temp_x_velocity_collision;
		y_velocity_collision = temp_y_velocity_collision;
		z_velocity_collision = temp_z_velocity_collision;
		x_dis_velocity_collision = temp_x_dis_velocity_collision;
		y_dis_velocity_collision = temp_y_dis_velocity_collision;
		z_dis_velocity_collision = temp_z_dis_velocity_collision;
	}
	/******************************************************************/
	
	
	// Update velocities	
	particle->x_velocity = x_velocity_vertical + x_dis_velocity_collision;
	particle->y_velocity = y_velocity_vertical + y_dis_velocity_collision;
	particle->z_velocity = z_velocity_vertical + z_dis_velocity_collision;
	
	set_velocity(target_particle, x_dis_velocity_vertical + x_velocity_collision,
									y_dis_velocity_vertical + y_velocity_collision,
									z_dis_velocity_vertical + z_velocity_collision);
}

void set_velocity(struct Particle *particle, int x, int y, int z){
	particle->x_velocity = x;
	particle->y_velocity = y;
	particle->z_velocity = z;
}

void draw_result(double* x_movement, double* y_movement, double* z_movement){
	int i, j;
	
	fprintf(output_file, "======================= Plot =====================");
	
	// Allocate map space for drawing
	bool** pic = (bool**)calloc(20, sizeof(bool*));
	for (i = 0; i < 20; i++){
		pic[i] = (bool*)calloc(50, sizeof(bool));
		memset(pic[i], false, 50);
	}
	
	double bias = pow(space_size, 1.0/3)/2;
	for (i = 0; i < run_time_step; i++){
		
		//Calculate position to draw on the map
		double x_pos = x_movement[i] - bias;
		double y_pos = y_movement[i] - bias - distance_of_2_particles/2;
		double z_pos = z_movement[i] - bias;
		
		x_pos = (x_pos*pow(10, 11)+10)/2;
		y_pos = (y_pos*pow(10, 11)+40)/2;
		z_pos = (z_pos*pow(10, 11)+30)/2;
		
		pic[(int)x_pos][(int)y_pos] = true;
	}
	
	// Draw the result into the file
	for (i = 0; i < 20; i++){
		for (j = 0; j < 50; j++){
			if (!pic[i][j]){
				fprintf(output_file, " ");
			}
			else{
				fprintf(output_file, "O");
			}
		}
		fprintf(output_file, "\n");
	}
	fprintf(output_file, "==================================================\n");
}