CC=gcc
CFLAGS=-Wall
LFLAGS=-lm
DEPS = common_math.h RNN.h
TEST_OBJ = test.o common_math.o RNN.o


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LFLAGS)

test: $(TEST_OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)

clean:
	rm $(TEST_OBJ)