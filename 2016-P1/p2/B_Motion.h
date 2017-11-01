#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


#define molecular_mass 28	// Pick N2
#define big_small_mass_ratial 100
#define particle_radius 1e-9
#define big_small_radius_ration 4.6

/* Environment Information */
/* Constance during process, need no to consider race condition */
int temperature;
double space_size;
double space_edge;
unsigned long long num_particles;
unsigned long long num_particles_edge;
double distance_of_2_particles;
double init_energy;
double init_velocity;
double gas_particles_mass;
double pollen_mass;
int run_time_step;
FILE* output_file;

/* Basic Information */
/* Might need no to consider race condition. */
int num_threads;
double** distance_table;

struct Particle{
	bool is_near_wall;
	unsigned long long index;
	double x_velocity;
	double y_velocity;
	double z_velocity;
	double x_pos;
	double y_pos;
	double z_pos;
	double mass;
	double velocity;
	double radius;
	double* distance_list;
};

// Particles!
struct Particle* particles;

// Functions
void input_data();
void initialize_particles();
void initialize(FILE* direction_file, struct Particle *particle, double in_mass, double in_velocity, double x, double y, double z, double in_radius, int in_index);
void show_environment_info();
void process();
void move(struct Particle *particle);
double get_distance(struct Particle *particle, struct Particle *target_particle);
void set_distance(struct Particle *particle, double value, unsigned long long index);
void collision(struct Particle *particle, struct Particle *target_particle);
void set_velocity(struct Particle *particle, int x, int y, int z);
void draw_result(double* x_movement, double* y_movement, double* z_movement);