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
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN := prog
PROF := prog_prof

.PHONY: all clean debug profile
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
