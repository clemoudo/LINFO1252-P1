CC ?= gcc
CFLAGS ?= -Wall -Wextra -g -std=c11 -Iinclude

SRC_DIR = src
BIN_DIR = bin
INCLUDE_DIR = include

# Liste des exécutables à générer (ex: main -> bin/main)
# Par défaut, compile src/main.c -> bin/main
TARGETS = $(BIN_DIR)/main

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
