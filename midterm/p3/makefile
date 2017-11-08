CC=gcc
CFLAGS=-W -Wall -Wextra -fopenmp
all:
	$(CC) $(CFLAGS) po_sum.c -o po_sum
	$(CC) $(CFLAGS) po_omp.c -o po_omp

clean:
	rm -f po_sum
