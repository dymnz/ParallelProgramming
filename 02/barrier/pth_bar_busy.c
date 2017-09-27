/* File: pth_bar.c
 * Input:
 *    none
 * Output:
 *    Time for BARRIER_COUNT barriers
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 10

int shared_count;
int thread_count;
pthread_mutex_t barrier_mutex;

void Usage(char* prog_name);
void *Thread_work(void* rank);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles; 
   double start, finish;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   thread_handles = malloc (thread_count*sizeof(pthread_t));
   //declare to use #thread_count barriers  
   pthread_mutex_init(&barrier_mutex, NULL);
   shared_count = 0;

   GET_TIME(start); //measure total time to synchronize all threads 
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);

   pthread_mutex_destroy(&barrier_mutex);
   free(thread_handles);
   return 0;
}  /* main */

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}  /* Usage */

void *Thread_work(void* rank) {
   long my_rank = (long) rank; 
   int i;           
   for (i = 0; i < BARRIER_COUNT; i++) {
      //at each iteration, must wait for all threads arriving this point!!! Use mutex+busy waiting
      pthread_mutex_lock(&barrier_mutex);
      shared_count++;
      pthread_mutex_unlock(&barrier_mutex);
      printf("At iteration %d, thread %ld arrives\n", i, my_rank);
      while (shared_count < thread_count);
      if (shared_count==thread_count-1)  //what happen?? Any problem??
         shared_count = 0;   // 1) As the last thread to reset shared counter, some other thread may never see it
                             // 2) If one thread rests shared counter after barrier and some other thread enters, its increment may be lost.
   }                
   return NULL;
}  /* Thread_work */
