#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 64;

/* Global variables:  accessible to all threads */
int thread_count;

// 2 threads, 1 state variable each
unsigned int state[2];
const int NUMr = 3;

void *mythread(void *rank) {
    long i;
    long myrank = (long) rank;
    unsigned int *mystate = &state[myrank];
    // XOR multiple values together to get a semi-unique seed
    *mystate = time(NULL) ^ myrank ^ pthread_self();

    for (i = 0; i < NUMr; i++) { 
      //printf("Th %ld > %u\n", myrank, rand());
      printf("Th %ld %u > %u\n", myrank, *mystate, rand_r(mystate));
    }	
    
    return NULL;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}  /* Usage */


int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles; 

   srand(time(NULL));	
   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   thread_handles = malloc(thread_count*sizeof(pthread_t));

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL,
          mythread, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }

   free(thread_handles);

   return 0;
}
