#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include "exerr.h"

#define NUM_PRODUCERS 10
#define NUM_CONSUMERS 3

#define MAX_SLEEP_TIME_PRODUCER 2
#define MAX_SLEEP_TIME_CONSUMER 3

#define SIZE_BUFFER 10
#define LIMIT_ITEM 1000

static int buffer[SIZE_BUFFER];
static int pos = 0;
static int prod_ids[NUM_PRODUCERS];
static int cons_ids[NUM_CONSUMERS];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cons_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t prod_cond = PTHREAD_COND_INITIALIZER;

void * produce (void *arg) {
  int id = *((int *) arg);
  int item;
  int r;

  while (1)
    {
      sleep (rand () % (MAX_SLEEP_TIME_PRODUCER + 1));
      item = rand () % LIMIT_ITEM;

      r = pthread_mutex_lock (&mutex);
      COND_MSG_ABORT (r, r, "Producer mutex lock");

      /* producers wait while buffer is full */
      while (pos == SIZE_BUFFER)
	{
	  /* will unlock mutex automatically */
	  printf ("Buffer is full: Producer %d waits\n", id);
	  r = pthread_cond_wait (&prod_cond, &mutex);
	  COND_MSG_ABORT (r, r, "Producer condwait");
	  printf ("Producer %d wakes up\n", id);
	  /* mutex locked automatically */
	}

      assert (pos >= 0);
      assert (pos < SIZE_BUFFER);

      printf ("Producer %d inserts %d\n", id, item);
      fflush (stdout);

      buffer[pos++] = item;

      r = pthread_cond_signal (&cons_cond);
      COND_MSG_ABORT (r, r, "Producer condsignal");
      r = pthread_mutex_unlock (&mutex);
      COND_MSG_ABORT (r, r, "Producer mutex unlock");
    }
  pthread_exit (NULL);
}

void *consume (void *arg) {
  int id = *((int *) arg);
  int item;
  int r;

  while (1)
    {
      sleep (rand () % (MAX_SLEEP_TIME_CONSUMER + 1));

      r = pthread_mutex_lock (&mutex);
      COND_MSG_ABORT (r, r, "Consumer mutex lock");

      /* consumers wait while buffer is empty */
      while (pos == 0)
	{
	  /* waiting will unlock mutex automatically */
	  printf ("Buffer is empty: Consumer %d waits\n", id);
	  r = pthread_cond_wait (&cons_cond, &mutex);
	  COND_MSG_ABORT (r, r, "Consumer condwait");
	  printf ("Consumer %d wakes up\n", id);
	  /* mutex locked automatically after waking up */
	}

      assert (pos > 0);
      assert (pos <= SIZE_BUFFER);
      item = buffer[pos - 1];
      pos--;
      printf ("Consumer %d gets item %d\n", id, item);
      fflush (stdout);

      r = pthread_cond_signal (&prod_cond);
      COND_MSG_ABORT (r, r, "Consumer condsignal");
      r = pthread_mutex_unlock (&mutex);
      COND_MSG_ABORT (r, r, "Consumer mutex unlock");
    }

  pthread_exit (NULL);
}

int main () {
  int i, r;
  pthread_t thread;

  srand (time (NULL));
  for (i = 0; i < NUM_PRODUCERS; i++)
    {
      prod_ids[i] = i;
      r = pthread_create (&thread, NULL, produce, &prod_ids[i]);
      COND_MSG_ABORT (r, r, "Thread creation");
    }
  for (i = 0; i < NUM_CONSUMERS; i++)
    {
      cons_ids[i] = i;
      r = pthread_create (&thread, NULL, consume, &cons_ids[i]);
      COND_MSG_ABORT (r, r, "Thread creation");
    }

  pthread_exit (NULL);
}
