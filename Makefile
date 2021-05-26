CC := gcc
CFLAGS := -Wall -O3 -march=native
ifeq ($(MAKECMDGOALS),debug)
	CFLAGS := -Wall -g -ggdb -DDEBUG -O0
else
	ifeq ($(MAKECMDGOALS),profile)
		CFLAGS := -Wall -g -pg
    endif
endif
BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDES :=
LFLAGS :=
LIBS := -lm
SRCS := $(wildcard $(SRC_DIR)/*.c)
BIG_INT_TEST_SRCS := test/test.c src/big_integer.c
BIG_INT_TEST_OBJS := $(BIG_INT_TEST_SRCS:.c=.o)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN := prog
PROF := prog_prof
BIG_INT_TEST := big_int_test

.PHONY: all clean debug profile big_int_test
$(MAIN): $(filter-out $(BUILD_DIR)/timing_add.o, $(OBJS))
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(filter-out $(BUILD_DIR)/timing_add.o $(wildcard $(BUILD_DIR)/big_integer_test*.o), $(OBJS)) $(LFLAGS) $(LIBS)

$(PROF): $(filter-out $(BUILD_DIR)/test.o, $(OBJS))
	$(CC) $(CFLAGS) $(INCLUDES) -o $(PROF) $(filter-out $(BUILD_DIR)/test.o, $(OBJS)) $(LFLAGS) $(LIBS)

all debug: $(MAIN)
profile: $(PROF)
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
clean:
	$(RM) -r build/
	$(RM) $(MAIN)
	$(RM) $(PROF)

big_int_test:
$(BIG_INT_TEST): $(BIG_INT_TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIG_INT_TEST) $(BIG_INT_TEST_OBJS) $(LFLAGS) $(LIBS)

TEST_DIR := ./test
BIG_INT_MUL_BENCHMARK_SRCS := test/timing_mul.c src/big_integer.c
BIG_INT_MUL_BENCHMARK_OBJS := build/test/timing_mul.o build/big_integer.o
BIG_INT_MUL_BENCHMARK := big_int_mul_benchmark
big_int_mul_benchmark:
$(BIG_INT_MUL_BENCHMARK): $(BIG_INT_MUL_BENCHMARK_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIG_INT_MUL_BENCHMARK) $(BIG_INT_MUL_BENCHMARK_OBJS) $(LFLAGS) $(LIBS)

BIG_FLOAT_BENCHMARK_SRCS := test/timing_bigfloat.c src/big_integer.c src/big_float.c src/big_float_helper.c
BIG_FLOAT_BENCHMARK_OBJS := build/test/timing_bigfloat.o build/big_integer.o build/big_float.o build/big_float_helper.o
BIG_FLOAT_BENCHMARK := big_float_benchmark
big_float_benchmark:
$(TEST_DIR)/$(BIG_FLOAT_BENCHMARK): $(BIG_FLOAT_BENCHMARK_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIG_FLOAT_BENCHMARK) $(BIG_FLOAT_BENCHMARK_OBJS) $(LFLAGS) $(LIBS)

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@