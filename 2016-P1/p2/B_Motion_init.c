#include "B_Motion.h"

void initialize_particles(){
	// Place pollen in the middle
	FILE* direction_file = fopen("init_direction.txt", "r");
	
	particles = (struct Particle*)calloc((num_particles + 1), sizeof(struct Particle));
	
	unsigned long long  i, j, k;
	for (i = 0; i < num_particles_edge; i++){
		for (j = 0; j < num_particles_edge; j++){
			for (k = 0; k < num_particles_edge; k++){
				unsigned long long  index = k + j*num_particles_edge + i*pow(num_particles_edge, 2);
				initialize(direction_file, &(particles[index]), gas_particles_mass, init_velocity, i*distance_of_2_particles, j*distance_of_2_particles, k*distance_of_2_particles, particle_radius, index);
			}
		}
	}
	
	initialize(direction_file, &(particles[num_particles]), pollen_mass, 0, pow(space_size, 1.0/3)/2, pow(space_size, 1.0/3)/2 + distance_of_2_particles/2, pow(space_size, 1.0/3)/2, particle_radius * big_small_radius_ration, num_particles);

	distance_table = (double**)calloc(num_particles+1, sizeof(double*));
	for (i = 0; i < num_particles+1; i++)
		distance_table[i] = (double*)calloc(num_particles+1, sizeof(double));
		
	for (i = 0; i < num_particles+1; i++){
		particles[i].distance_list = distance_table[i];
	}
}

void input_data(){
	
	gas_particles_mass = molecular_mass/(6.02e23);
	pollen_mass = big_small_mass_ratial * molecular_mass/(6.02e23);
	
	temperature = 70;
	printf("Temperature is %d degree.\n", temperature);
	temperature += 273.15;
	
	init_energy = (3.0/2)* 1.38e-23 * temperature;
	init_velocity = sqrt(2*init_energy/gas_particles_mass);
	
	printf("Space is %.3lf (in 0.0001 * micrometer^3)\n", space_size);
	
	num_particles_edge = (unsigned long long)pow((space_size * (1000)), 1.0/3);
	num_particles = pow(num_particles_edge, 3);
	space_size *= 1e-22;
	distance_of_2_particles = pow(space_size, 1.0/3)/num_particles_edge;
	space_edge = pow(space_size, 1.0/3);
	
	run_time_step = 10;
	printf("Number of time step is %d ( per 1e-11 sec )\n", run_time_step);
	
}

void initialize(FILE* direction_file, struct Particle *particle, double in_mass, double in_velocity, double x, double y, double z, double in_radius, int in_index){
	particle->mass = in_mass; 
	particle->velocity = in_velocity; 
	particle->x_pos = x; 
	particle->y_pos = y; 
	particle->z_pos = z; 
	particle->radius = in_radius; 
	particle->index = in_index;

	double x_dir, y_dir, z_dir;
	
	fscanf(direction_file, "%lf", &x_dir);
	fscanf(direction_file, "%lf", &y_dir);
	fscanf(direction_file, "%lf", &z_dir);
	
	particle->x_velocity = particle->velocity*x_dir;
	particle->y_velocity = particle->velocity*y_dir;
	particle->z_velocity = particle->velocity*z_dir;
}

void show_environment_info(){
	printf("\nEnvironment Status:\n");
	printf("\tNumber of gas particles: %llu\n", num_particles);
	printf("\tKinetic energy of each gas particle: %e (J)\n", init_energy);
	printf("\tVelocity of each gas particle: %lf (m/s)\n", init_velocity);
	printf("\tMass of a gas particle: %e (kg)\n", gas_particles_mass);
	printf("\tMass of the pollen: %e (kg)\n", pollen_mass);
}