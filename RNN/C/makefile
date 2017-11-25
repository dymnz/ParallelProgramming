CC=gcc
CFLAGS=-Wall
DEPS = common_math.h
TEST_OBJ = test.o common_math.o


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TEST_OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm $(TEST_OBJ)