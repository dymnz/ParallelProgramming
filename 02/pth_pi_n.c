/* File:     pth_pi.c
 * Purpose:  Try to estimate pi using the formula
 */        

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

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

   /* Get number of threads from command line */
   Get_args(argc, argv);
   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 

   sum = Serial_pi(n);
   printf("%Lf\n", sum)
   while(1);
   long double thread_sum = 0.0;
   long double term, diff;
   int rotation = 0;
   while(1) {
      for (thread = 0; thread < thread_count; thread++)  
         pthread_create(&thread_handles[thread], NULL,
                        Thread_sum, (void*)&thread);     
      for (thread = 0; thread < thread_count; thread++) {
         pthread_join(thread_handles[thread], (void **) &term); 
         thread_sum += term;
         diff = thread_sum - sum > 0? thread_sum - sum : -(thread_sum - sum);
         if ( diff < 1e-20) {
            printf("%d\n", (rotation * thread_count) + thread);
         }
      }
      ++rotation;
   }
   
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
   long length = *((long  *) n);
   long double eight_n = 8.0 * length;
   long double factor = 1.0, term;
   int i;

   for (i = 0; i < length ; ++i)
      factor /= 16.0;

   term = (
      factor * (
           4/(eight_n+1) 
         - 2/(eight_n+4) 
         - 1/(eight_n+5) 
         - 1/(eight_n+6)));
   return (void *) term;

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
   long double eight_n = 8.0 * length;   
   long double i;

   for (i = 0; i < term ; ++i) {
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
