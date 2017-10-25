CC=gcc
CFLAGS=-W -Wall -Wextra -fopenmp
all:
  $(CC) $(CFLAGS) -o flush flush.c
  $(CC) $(CFLAGS) -o flush2 flush2.c
	$(CC) $(CFLAGS) -o nbody nbody.c

clean:
	rm -f flush flush2 nbody
