CC=gcc
CFLAGS=-W -Wall -Wextra -fopenmp
all:
	$(CC) $(CFLAGS) -o orderCollapse orderCollapse.c
	$(CC) $(CFLAGS) -o threadPrivate threadPrivate.c
	$(CC) $(CFLAGS) -o firstLast firstLast.c
	$(CC) $(CFLAGS) -o defaultReduce defaultReduce.c
	$(CC) $(CFLAGS) -o mandel mandel.c
	$(CC) $(CFLAGS) -o mandel_sol mandel_sol.c
	$(CC) $(CFLAGS) -o flush flush.c
	$(CC) $(CFLAGS) -o flush2 flush2.c

clean:
	rm -f orderCollapse threadPrivate firstLast defaultReduce mandel mandel_sol flush flush2
