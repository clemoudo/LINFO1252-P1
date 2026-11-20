CC = gcc
CFLAGS = -Wall -Wextra -O0 -pthread -Iinclude

SRCS = $(wildcard src/*.c)
BINS = $(patsubst src/%.c,bin/%,$(SRCS))

all: $(BINS)

bin/%: src/%.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin:
	mkdir -p bin

test: all
	./bin/philosophes 2
	./bin/prodcons 2 2
	./bin/read-write 2 2

clean:
	rm -rf bin

.PHONY: all clean test
