#### MPI 
```
    #include <mpi.h>

	MPI_Init(&argc, &argv);      /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */
    MPI_Finalize();     /* stop all process */
```


#### Communication

Blocking: 
``` 
    int MPI_Send(
        const void *buf, 
        int count, 
        MPI_Datatype datatype, 
        int dest, 
        int tag, 
        MPI_Comm comm
    );

    int MPI_Recv(
        void *buf,
        int count,
        MPI_Datatype datatype,
        int source,
        int tag,
        MPI_Comm comm,
        MPI_Status *status
    );
```