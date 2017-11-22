/****************************************************************************
 * nbody.c
 * An implementation of the naive O(n^2)/timestep numerical integration
 * approach to the N-body problem.  Parallelism via OpenMP.
 * by Sean Ho
 ****************************************************************************/

#include "nbody.h"
#include "pthread.h"

#define DEFAULT_THREAD_NUM 4

Body *bodies;

//#define PRIVATE_VARS r, dist, force_len, force_ij, tot_force_i, dv
struct Thread_Param {
	int i, num_bodies;
	double dv[2], dt;
	double r[2], dist, force_len, force_ij[2], tot_force_i[2];

};

int main(int argc, char **argv) {

	mytspec start_time, end_time;
	int num_bodies, num_steps, max_threads = 1;
	int i, j, k, l;
	double dt = 1.0;
	pthread_t *thread_handle_list;
	struct Thread_Param *thread_param_list;

#if NEWTON_OPT
	double *forces_matrix;
#endif

	/* Parse command line */
	const char Usage[] = "Usage: nbody <num bodies> <num_steps> <num_threads>\n";
	if (argc < 2) {
		fprintf(stderr, Usage);
		exit(1);
	}

	num_bodies = atoi(argv[1]);
	num_steps = atoi(argv[2]);

	if (argc > 2)
		max_threads = atoi(argv[3]);
	else
		max_threads = DEFAULT_THREAD_NUM;

	// Create thread handles
	thread_handle_list = (pthread_t*)
	                     malloc(max_threads * sizeof(pthread_t));

	// Private param list
	thread_param_list = (struct Thread_Param*)
	                    malloc(max_threads * sizeof(struct Thread_Param));

#if NEWTON_OPT > 0
	printf("Using Newton's third law optimization, variant %d.\n", NEWTON_OPT);
#endif

	/**************************************************************************
	 * Initialize bodies and allocate internal data structures
	 *************************************************************************/

	printf("Initializing with %d threads, %d bodies, %d time steps\n",
	       max_threads, num_bodies, num_steps);

	bodies = init_bodies(num_bodies, INIT_SPIRAL);
	check_simulation(bodies, num_bodies);

#if NEWTON_OPT == 1
	forces_matrix = (double *) malloc(sizeof(double) * 2 * num_bodies * num_bodies);
#elif NEWTON_OPT == 2
	forces_matrix = (double *) malloc(sizeof(double) * 2 * num_bodies);
#endif

	/* Start timing */
	printf("Running ");
	fflush(stdout);
	get_time(start_time);

	/**************************************************************************
	 * Main loop
	 *************************************************************************/

#define PRIVATE_VARS r, dist, force_len, force_ij, tot_force_i, dv

	/* Store in force_ij[] the force on body i by body j */
#define Calc_Force_ij() \
  thread_param->r[X] = bodies[j].pos[X] - bodies[thread_param->i].pos[X]; \
  thread_param->r[Y] = bodies[j].pos[Y] - bodies[thread_param->i].pos[Y]; \
  thread_param->dist = \
  	thread_param->r[X]*thread_param->r[X] + \
  	thread_param->r[Y]*thread_param->r[Y]; \
  thread_param->force_len = \
  	GRAV_CONST * bodies[thread_param->i].mass * bodies[j].mass  \
    / (thread_param->dist*sqrt(thread_param->dist)); \
  thread_param->force_ij[X] = \
  	thread_param->force_len * thread_param->r[X]; \
  thread_param->force_ij[Y] = \
  	thread_param->force_len * thread_param->r[Y]

	/* Update velocity and position of body i, by numerical integration */

	//#define PRIVATE_VARS r, dist, force_len, force_ij, tot_force_i, dv
#define Step_Body_i() \
  thread_param->dv[X] = \
  	thread_param->dt * thread_param->tot_force_i[X] \
  	/ bodies[thread_param->i].mass; \
  thread_param->dv[Y] = \
  	thread_param->dt * thread_param->tot_force_i[Y] \
  	/ bodies[thread_param->i].mass; \
  bodies[thread_param->i].pos[X] += \
  	thread_param->dt * ( bodies[thread_param->i].vel[X] \
  		+ thread_param->dv[X]/2 ); \
  bodies[thread_param->i].pos[Y] += \
  	thread_param->dt * ( bodies[thread_param->i].vel[Y] \
  		+ thread_param->dv[Y]/2 ); \
  bodies[thread_param->i].vel[X] += \
  	thread_param->dt * thread_param->dv[X]; \
  bodies[thread_param->i].vel[Y] += \
  	thread_param->dt * thread_param->dv[Y]

	for (k = 0; k < num_steps; k++) {
		printf(". ");
		fflush(stdout);

#if !NEWTON_OPT

		int thread_i;

		// This is dumb
		for (thread_i = 0; thread_i < max_threads; ++i, ++thread_i)
			thread_param_list[thread_i].num_bodies = num_bodies;

		//#define PRIVATE_VARS r, dist, force_len, force_ij, tot_force_i, dv
		for (i = 0; i < num_bodies; ) {
			for (
			    thread_i = 0;
			    thread_i < max_threads && i < num_bodies;
			    ++i, ++thread_i) {
				thread_param_list[thread_i].i = i;
				pthread_create(&thread_handle_list[thread_i],
				               NULL, compute_job_1, &thread_param_list[thread_i]);
			}
			for (thread_i = 0; thread_i < max_threads; ++thread_i)
				pthread_join(thread_handle_listddddd[thread_i], NULL);
		}
		/*
		#pragma omp parallel for private(j, PRIVATE_VARS)
		for (i = 0; i < num_bodies; i++) {
			tot_force_i[X] = 0.0;
			tot_force_i[Y] = 0.0;

			//Compute total force f(i) on each body i

			for (j = 0; j < num_bodies; j++) {
				if (j == i) continue;

				Calc_Force_ij();

				tot_force_i[X] += force_ij[X];
				tot_force_i[Y] += force_ij[Y];
			}

			Step_Body_i();
		}*/

#elif NEWTON_OPT == 1

#define forces(i,j,x) forces_matrix[x + 2*(j + num_bodies*i)]

		/* Fill in a big nxn table of forces */
		#pragma omp parallel for private(j, PRIVATE_VARS)
		for (i = 0; i < num_bodies; i++) {
			for (j = i + 1; j < num_bodies; j++) {
				Calc_Force_ij();

				forces(i, j, X) = force_ij[X];
				forces(i, j, Y) = force_ij[Y];
			}
		}

		/* Compute total force f(i) on each body i */
		#pragma omp parallel for private(j, PRIVATE_VARS)
		for (i = 0; i < num_bodies; i++) {
			tot_force_i[X] = 0.0;
			tot_force_i[Y] = 0.0;

			for (j = 0; j < i; j++) {
				tot_force_i[X] -= forces(j, i, X);
				tot_force_i[Y] -= forces(j, i, Y);
			}
			for (j = i + 1; j < num_bodies; j++) {
				tot_force_i[X] += forces(i, j, X);
				tot_force_i[Y] += forces(i, j, Y);
			}

			/* Update velocity and position of each body, by numerical integration */
			Step_Body_i();
		}

#elif NEWTON_OPT == 2

#define forces(i,x) forces_matrix[x + 2*i]

		#pragma omp parallel for private(j, PRIVATE_VARS)
		for (i = 0; i < num_bodies; i++) {
			for (j = i + 1; j < num_bodies; j++) {
				Calc_Force_ij();

				#pragma omp critical
				{
					forces(i, X) += force_ij[X];
					forces(i, Y) += force_ij[Y];
					forces(j, X) -= force_ij[X];
					forces(j, Y) -= force_ij[Y];
				}
			}
		}

		#pragma omp parallel for private(j, PRIVATE_VARS)
		for (i = 0; i < num_bodies; i++) {
			Step_Body_i();
		}

#endif /* !NEWTON_OPT */

	}

	/* Stop timing */
	get_time(end_time);
	check_simulation(bodies, num_bodies);

	printf("done!  interaction rate: \n%6.0f\n",
	       num_bodies * (num_bodies - 1) * num_steps /
	       elapsed_time(end_time, start_time) / 1000);

	return (0);
}

//#define PRIVATE_VARS r, dist, force_len, force_ij, tot_force_i, dv
void *compute_job_1(void *param) {
	int j;
	struct Thread_Param *thread_param = (struct Thread_Param *) param;

	thread_param->tot_force_i[X] = 0.0;
	thread_param->tot_force_i[Y] = 0.0;

	/* Compute total force f(i) on each body i */
	for (j = 0; j < thread_param->num_bodies; j++) {
		if (j == thread_param->i) continue;

		Calc_Force_ij();

		thread_param->tot_force_i[X] += thread_param->force_ij[X];
		thread_param->tot_force_i[Y] += thread_param->force_ij[Y];
	}

	Step_Body_i();
}

/****************************************************************************
 * Allocate and setup initial conditions for the bodies
 ****************************************************************************/
Body *init_bodies(unsigned int num_bodies, int init_type) {
	int i;
	double n = num_bodies;
	Body *bodies = (Body *) malloc(num_bodies * sizeof(Body));

	for (i = 0; i < num_bodies; i++) {
		switch (init_type) {
		case INIT_LINEAR:
			bodies[i].mass = 1.0;
			bodies[i].pos[X] = i / n;
			bodies[i].pos[Y] = i / n;
			bodies[i].vel[X] = 0.0;
			bodies[i].vel[Y] = 0.0;
			break;
		case INIT_SPIRAL:
			bodies[i].mass = (n - i) / n;
			bodies[i].pos[X] = (1 + i / n) * cos(2 * M_PI * i / n) / 2;
			bodies[i].pos[Y] = (1 + i / n) * sin(2 * M_PI * i / n) / 2;
			bodies[i].vel[X] = 0.0;
			bodies[i].vel[Y] = 0.0;
			break;
		}
	}

	return bodies;
}

/****************************************************************************
 * Verify that the simulation is running correctly:
 * it should satisfy the invariant of conservation of momentum
 ****************************************************************************/
int check_simulation(Body *bodies, int num_bodies) {
	int i, check_ok;
	double momentum[2] = { 0.0, 0.0 };

	for (i = 0; i < num_bodies; i++) {
		momentum[X] += bodies[i].mass * bodies[i].vel[X];
		momentum[Y] += bodies[i].mass * bodies[i].vel[Y];
	}

	check_ok = ((abs(momentum[X]) < EPSILON) && (abs(momentum[Y]) < EPSILON));
	if (!check_ok) printf("Warning: total momentum = (%3.3f, %3.3f)\n",
		                      momentum[X], momentum[Y]);
	return check_ok;
}

/****************************************************************************
 * elapsed time in seconds for POSIX-compliant clocks
 ****************************************************************************/
#ifdef GNU_TIME
double elapsed_time(const mytspec t2, const mytspec t1) {
	return 1.0 * (t2 - t1) / CLOCKS_PER_SEC;
}
#else
double elapsed_time(const mytspec t2, const mytspec t1) {
	return (((double)t2.tv_sec) + ((double)t2.tv_nsec / 1e9))
	       - (((double)t1.tv_sec) + ((double)t1.tv_nsec / 1e9));
}
#endif

