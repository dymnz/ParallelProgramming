CC=gcc
CFLAGS=-W -Wall -Wextra
all:
	$(CC) $(CFLAGS) -lpthread pth_bar.c -o bar
	$(CC) $(CFLAGS) -lpthread pth_bar_bymu.c -o bar_bymu
	$(CC) $(CFLAGS) -lpthread pth_bar_busy.c -o bar_busy
	$(CC) $(CFLAGS) -lpthread pth_bar_cond.c -o bar_cond
	$(CC) $(CFLAGS) -lpthread condvar.c -o condvar

clean:
	rm -f bar bar_bymu bar_busy bar_cond cond_var
