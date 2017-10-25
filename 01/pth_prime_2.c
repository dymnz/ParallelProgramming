/* 
   Range spliced into N part by increment N when iterating in each thread
   TODO: Format the output in increasing order
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define VERBOSE
//#define DEBUG
#define MAX_THREAD 64

struct Primecheck_args {
   int lower_limit;
   int upper_limit;
   int skip;
};

void *primecheck(void *param); /* the thread */

int main(int argc, char *argv[]) {

   // Verify two args were passed in
   if (argc < 3) {
      fprintf(stderr, "USAGE: ./prime <Integer value> <# of Thread>\n");
      exit(1);
   }

   int max_num = atoi(argv[1]);
   if (max_num < 2) {
      fprintf(stderr, "USAGE: %d must be >= 2\n", atoi(argv[1]));
      exit(1);
   }

   int max_thread = atoi(argv[2]);
   if (max_thread < 1 || max_thread > MAX_THREAD) {
      fprintf(stderr, "1 <= #_of_thread <= 64");
      exit(1);
   }

   pthread_t *thread_list 
      = (pthread_t *) malloc(sizeof(pthread_t) * max_thread); 
   struct Primecheck_args *arg_list = (struct Primecheck_args*) malloc(sizeof(struct Primecheck_args) * max_thread);

   int i;
   int segment_size = max_num / max_thread;


   // Create the threads
   for (i = 0; i < max_thread; ++i) {
      arg_list[i].lower_limit = (i * 2) + 1;

      // Old: 
      // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
      // 1       5       9          13
      //   2       6       10          14
      //     3       7        11          15
      //       4       8         12    

      // New:
      // 1 3 5 7 9 11 13 15 17 19 21 23 25
      // 1       9          17          25
      //   3       11          19    
      //     5        13          21     
      //       7         15          23

      arg_list[i].upper_limit = max_num; 
      arg_list[i].skip = max_thread * 2;

      pthread_create(&(thread_list[i]), NULL, primecheck, (void *) &arg_list[i]);
   }
   
   // Wait for the thread to exit
   //printf("Prime Numbers: ");
   for (i = 0; i < max_thread; ++i)
      pthread_join(thread_list[i], NULL);


   printf("\nComplete\n");
}

// The thread will begin control in this function
void *primecheck(void *param) {
   int is_prime = 1;
   int i, j, check_ceil;
   struct Primecheck_args *params = (struct Primecheck_args*) param;

   #ifdef DEBUG
   printf("Lower: %6d \t Upper: %6d\n", 
      params->lower_limit, 
      params->upper_limit);
   #endif

   /* Check to see if a number is prime */
   for (i = params->lower_limit; 
         i <= params->upper_limit; 
         i += params->skip) {

      /* If the number is even, it's not a prime*/
      if (i != 2 && (i & 1) == 0) {
         continue;
      }

      /* From 3:2:sqrt(i), check if i can be divided by any number */
      is_prime = 1;
      check_ceil = (int) ceil(sqrt(i));
      for (j = 3; j <= check_ceil; j+=2) {
         int result = i % j;
         if (result == 0) {
            is_prime = 0;
            break;
         }
      }

      // No numbers divided cleanly so this number must be prime
      #ifdef VERBOSE
      if (is_prime == 1) {
         printf("[%d] ", i);
      }
      #endif

   }
   // Exit the thread
   pthread_exit(0);
}
