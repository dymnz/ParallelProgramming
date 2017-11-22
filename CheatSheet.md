### Pthread

*from https://jlmedina123.wordpress.com/2014/05/29/multhreading-and-pthreads-api-cheatsheet/*

#### Pthread creation:
* pthread_t threads[N]
* pthread_create(&threads[i], NULL, start_routine, void *args)
* pthread_join(threads[i], void **return_value)

```
void *start_routine(void *args) {
    int *value = (int *) malloc(sizeof(int));
    value = 100;
    return &value;
}
```

#### Mutex:
* pthread_mutex_t mutex;
* pthread_mutex_init(&mutex);
* pthread_mutex_lock(&mutex);
* pthread_mutex_unlock(&mutex);
* pthread_mutex_destroy(&mutex);

#### Semaphore:
* sem_t sem;
* sem_init(&sem, 0, initial) -> initial = 0: lock, initial > 0: unlocked
* sem_wait(&sem) -> sem = 0: wait, sem > 0 decrement and go
* sem_post(&sem) -> increment value
* sem_destroy(&sem)


#### Condition variable:
* pthread_cond_t cond
* pthread_cond_init(&cond)
* pthread_cond_wait(&cond, &mutex) -> unlock mutex and wait on cond
* pthread_cond_signal(&cond) -> wake up threads waiting on cond
* pthread_cond_destroy(&cond)


#### Common condition variable usage:
* pthread_mutex_lock(&mutex);
* while(isnotready()) pthread_cond_wait(&cond, &mutex);
* critical section
* pthread_mutex_unlock(&mutex);
* pthread_cond_signal(&cond2);

#### Read write lock:
* pthread_rwlock_t rwlock
* pthread_rwlock_init(&rwlock, NULL)
* pthread_rwlock_rdlock(&rwlock)
* pthread_rwlock_wrlock(&rwlock)

