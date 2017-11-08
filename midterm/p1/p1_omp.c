// Copyright (C) 2009, Armin Biere, JKU, Linz, Austria.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <math.h>

static int verbose;
static double mods;

int main (int argc, char ** argv){
	long n = 0, m, t, res, i;
	char * sieve;
	int thread_num;

	/** Parsing argv **/
	if (argc < 3){
		printf("Usage: ./%s <thread_num> <number>\n", argv[0]);
		return 0;
	}
	
	if ((thread_num = atoi (argv[1])) <= 0){
		printf("invalid thread number\n");
		return -1;
	}
	
	if ((n = atoi (argv[2])) <= 0){
		printf("invalid number\n");
		return -1;
	}


	
	/** Initialization **/
	sieve = calloc (n + 1, 1);
	res = 0;
	
	/** Setup timer **/
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
/***************************************************************/
/******************  Midterm Exam Starts Here  *****************/

	// set thread num here

	for (m = 2; m <= n; m++) {
		if (sieve[m])
			continue;
		res++;
		for (t = 2 * m; t <= n; t += m)
			sieve[t] = 1;
	}
/******************  Midterm Exam Ends Here  *****************/
/***************************************************************/

	/** Calculate Time **/
	gettimeofday(&end, NULL);
	double timeInterval = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

	/** Get memory usage **/
	struct rusage r_usage;
	getrusage(RUSAGE_SELF,&r_usage);
	
	/** Output performence **/
	printf("Memory usage: %ld bytes\n", r_usage.ru_maxrss);
	printf("Time: %lf sec\n", timeInterval);

	free (sieve);
	printf ("%d\n", res);

  return 0;
}
