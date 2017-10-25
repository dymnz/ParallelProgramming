CC=gcc
CFLAGS=-W -Wall -Wextra -fopenmp
all:
	$(CC) $(CFLAGS) omp_hello.c -o hello
	$(CC) $(CFLAGS) omp_parfor.c -o parfor
	$(CC) $(CFLAGS) omp_pfor.c -o pfor
	$(CC) $(CFLAGS) omp_schedule.c -o schedule
	$(CC) $(CFLAGS) omp_parsec.c -o parsec
	$(CC) $(CFLAGS) omp_thdpri.c -o thdpri
	$(CC) $(CFLAGS) omp_reduce.c -o reduce
	$(CC) $(CFLAGS) omp_criatm.c -o criatm
	$(CC) $(CFLAGS) mm.c -o mm
	$(CC) $(CFLAGS) omp_mm.c -o omp_mm
	$(CC) $(CFLAGS) pi.c -o pi

clean:
	rm -f hello parfor pfor schedule parsec thdpri reduce criatm mm omp_mm pi
