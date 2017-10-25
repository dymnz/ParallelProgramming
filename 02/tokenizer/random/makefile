CC=gcc
CFLAGS=-W -Wall -Wextra -lpthread
all:
	$(CC) $(CFLAGS) pth_rand_r.c -o prand_r
	$(CC) $(CFLAGS) pth_rand_notsafe.c -o prand
	$(CC) $(CFLAGS) pth_rand_thdsafe.c -o prand_safe

clean:
	rm -f prand_r prand prand_safe
