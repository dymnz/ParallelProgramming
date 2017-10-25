CC=gcc
CFLAGS=-W -Wall -Wextra
all:
	$(CC) $(CFLAGS) -pthread pth_tokenize.c -o tokenize
	$(CC) $(CFLAGS) -pthread pth_tokenize_r.c -o tokenize_r

clean:
	rm -f tokenize tokenize_r
