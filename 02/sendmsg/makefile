CC=gcc
CFLAGS=-W -Wall -Wextra
all:
	$(CC) $(CFLAGS) -pthread pth_msg_ori.c -o msg_ori
	$(CC) $(CFLAGS) -pthread pth_msg_busy.c -o msg_busy
	$(CC) $(CFLAGS) -pthread pth_msg_sem.c -o msg_sem

clean:
	rm -f msg_ori msg_busy msg_sem
