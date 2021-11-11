.PHONY: all clean test random

CC := clang++
# CC := xcrun ./OpenCilk-10.0.01-Darwin/bin/clang
LD := ld.lld
CFLAGS := -g -std=c++17 -fopencilk -O3
# CFLAGS := -g -std=c++17 -fopencilk -O3 -fsanitize=cilk -fcilktool=cilkscale
SOURCES := $(shell find ./ -name "*.cpp")
TARGET := main

TEST_NUM ?= 5
INPUT_MAX_LEN ?= 1000

%: %.cpp $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) 

all: $(SOURCES) $(TARGET)

clean:
	rm $(TARGET)

LIST = $(shell seq 1 ${TEST_NUM})
random:
	python3 gen_testfile.py $(TEST_NUM) $(INPUT_MAX_LEN)

test:
	for i in $(LIST); do \
		./main ./input/$$i.txt ; \
	done