.PHONY: all clean test random

CC := clang++
# CC := xcrun ./OpenCilk-10.0.01-Darwin/bin/clang
LD := ld.lld
CFLAGS := -g -std=c++17 -fopencilk -O3
# CFLAGS := -g -std=c++17 -fopencilk -O3 -fsanitize=cilk -fcilktool=cilkscale
SOURCES := $(shell find ./ -name "*.cpp")
TARGET := main

INPUT_MAX_LEN ?= 700

%: %.cpp $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) 

all: $(SOURCES) $(TARGET)

clean:
	rm $(TARGET) input.txt

LIST = $(shell seq 1 ${TEST_NUM})
random:
	python3 gen_testfile.py 1 $(INPUT_MAX_LEN)

test:
	./main ./input.txt