/* File:     pth_pi.c
 * Purpose:  Try to estimate pi using the formula
 */        

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

//#define STEP_VERBOSE

//#define FIND_K                // Stop until precision reached
#define COMPARE_SPEED         // Stop until deisred term reached

#ifdef FIND_K
#define MAX_PRECISION 1e-10
#endif 


const int MAX_THREADS = 1024;

long thread_count;
long long n;

void* Thread_sum(void* rank);

/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
long double Serial_pi(long long n);

int main(int argc, char* argv[]) {
   long       thread;  /* Use long in case of a 64-bit system */
   pthread_t* thread_handles;

   long double serial_sum = 0.0;
   long double thread_sum = 0.0;

   clock_t clock_begin, clock_elapsed;

   /* Get number of threads from command line */
   Get_args(argc, argv);
   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 


   printf("Starting timer for serial\n");
   clock_begin = clock();
   serial_sum = Serial_pi(n);
   clock_elapsed = clock() - clock_begin;
   printf("Clock elapsed for serial: %f\n", (double) clock_elapsed);
   printf("Serial pi: %Le\n", serial_sum);

   #ifdef FIND_K
   long double diff;
   #endif 
   long double *term_ptr;
   long current_term;
   int rotation = 0, go = 1;

   printf("Starting timer for pthread\n");
   clock_begin = clock();
   while (go) {
      // Create
      for (thread = 0; thread < thread_count; thread++)  {
         current_term = (rotation * thread_count) + thread;

         #ifdef COMPARE_SPEED
            if (current_term > n) {
               go = 0;
               break;
            }
         #endif 

         pthread_create(&thread_handles[thread], NULL,
                        Thread_sum, (void*)current_term);    
         #ifdef STEP_VERBOSE
         printf("thread: %ld handling %ld term\n", thread, current_term);
         #endif 
      }

      // Join
      for (thread = 0; thread < thread_count; thread++) {
         current_term = (rotation * thread_count) + thread;

         #ifdef COMPARE_SPEED
            if (current_term > n) {
               go = 0;
               break;
            }
         #endif 

         pthread_join(thread_handles[thread], (void **) &term_ptr); 

         thread_sum += *term_ptr;

         #ifdef STEP_VERBOSE
         printf("From thread: %ld, the calcualted term is %Le\n", thread, *term_ptr);
         printf("Current sum: %Lf\n", thread_sum);
         sleep(1);
         #endif
         
         free(term_ptr);
         
         #ifdef FIND_K
         // abs(diff)
         diff = thread_sum - serial_sum > 0? 
            thread_sum - serial_sum 
            : -(thread_sum - serial_sum);

         if (diff < MAX_PRECISION) {
            #ifdef STEP_VERBOSE
            printf("Desired precision reached with %ld terms\n", (rotation * thread_count) + thread);            
            #endif
            go = 0;
            break;
         }
         #endif
      }
      ++rotation;
   }
   clock_elapsed = clock() - clock_begin;   
   printf("Clock elapsed for pthread: %lf\n", (double) clock_elapsed);
   printf("Thread pi: %Le\n", thread_sum);
   
   free(thread_handles);
   return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:       Thread_sum
 * Purpose:        Add in the terms computed by the thread running this 
 * In arg:         rank
 * Ret val:        ignored
 * Globals in:     n, thread_count
 * Global in/out:  sum 
 */
void* Thread_sum(void* n) {
   long length = (long) n;
   long double eight_n = 8.0 * length;
   long double factor = 1.0;
   long double *term_ptr = (long double *) malloc(sizeof(long double));

   int i;
   for (i = 0; i < length ; ++i)
      factor /= 16.0;

   *term_ptr = (
      factor * (
           4/(eight_n+1) 
         - 2/(eight_n+4) 
         - 1/(eight_n+5) 
         - 1/(eight_n+6)));

   return (void *) term_ptr;

}  /* Thread_sum */

/*------------------------------------------------------------------
 * Function:   Serial_pi
 * Purpose:    Estimate pi using 1 thread
 * In arg:     n
 * Return val: Estimate of pi using n terms of Maclaurin series
 */
long double Serial_pi(long long length) {
   long double sum = 0.0;
   long double factor = 1.0;
   long double eight_n, term;      
   long i;

   for (i = 0; i < length ; ++i) {
      eight_n = 8.0 * i;
      term = (
         factor * (
              4/(eight_n+1) 
            - 2/(eight_n+4) 
            - 1/(eight_n+5) 
            - 1/(eight_n+6)));      
      sum += term;

      factor /= 16.0;
   }


   return sum;
}  /* Serial_pi */

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */
