CC := gcc
CFLAGS := -Wall -O3
ifeq ($(MAKECMDGOALS),debug)
	CFLAGS := -Wall -g -ggdb -DDEBUG
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
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(filter-out $(BUILD_DIR)/timing_add.o, $(OBJS)) $(LFLAGS) $(LIBS)

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
