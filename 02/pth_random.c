#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>


// 2 threads, 1 state variable each
unsigned int state[2];

void *mythread(void *p_mystate)
{
    unsigned int *mystate = p_mystate;
    // XOR multiple values together to get a semi-unique seed
    *mystate = time(NULL) ^ getpid() ^ pthread_self();

    //...
    int rand1 = rand_r(mystate);
    //...
    int rand2 = rand_r(mystate);
    //...
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    // give each thread the address of its state variable
    pthread_create(&t1, NULL, mythread, &state[0]);
    pthread_create(&t2, NULL, mythread, &state[1]);
    //...
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
