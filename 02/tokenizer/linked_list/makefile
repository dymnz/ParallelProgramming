CC=gcc
CFLAGS=-W -Wall -Wextra
all:
	$(CC) $(CFLAGS) linked_list.c -o ll
	$(CC) $(CFLAGS) -pthread pth_ll_one_mut.c my_rand.c -o ll_om
	$(CC) $(CFLAGS) -pthread pth_ll_mult_mut.c my_rand.c -o ll_mm
	$(CC) $(CFLAGS) -pthread pth_ll_rwl.c my_rand.c -o ll_rw 

clean:
	rm -f ll ll_om ll_mm ll_rw
