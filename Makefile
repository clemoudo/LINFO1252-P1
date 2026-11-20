CC = gcc
CFLAGS = -Wall -Wextra -O0 -pthread

# Détection de Python (.venv en priorité)
PYTHON ?= $(shell if [ -f .venv/bin/python3 ]; then echo .venv/bin/python3; else echo python3; fi)

# Détection automatique des données studsrv les plus récentes dans data/
LATEST_STUDSRV = $(shell ls -td data/studsrv_* 2>/dev/null | head -n 1)
DATA ?= $(if $(LATEST_STUDSRV),$(LATEST_STUDSRV),data)

# Cible pour le benchmark (par défaut: all)
TARGET ?= all

# Fichier de feedback par défaut
FEEDBACK ?= feedback_studsrv.txt

# Sources et binaires Partie 1
SRCS_P1 = $(wildcard src/part1/*.c)
BINS_P1 = $(patsubst src/part1/%.c,bin/%,$(SRCS_P1))

all: $(BINS_P1)

bin/%: src/part1/%.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin:
	mkdir -p bin

test: all
	./bin/philosophes 2
	./bin/producer_consumer 2 2
	./bin/reader_writer 2 2

benchmark: all
	./src/part1/benchmark_part1.sh $(TARGET) | ./data/save_data.sh

save-studsrv:
	@if [ -f "$(FEEDBACK)" ]; then \
		./data/save_data.sh --studsrv < "$(FEEDBACK)"; \
	else \
		echo "Collez le feedback studsrv ci-dessous (terminez avec Ctrl+D) :"; \
		./data/save_data.sh --studsrv; \
	fi

plot:
	$(PYTHON) graphes/generate_plot.py $(DATA)

clean:
	rm -rf bin

zip: clean
	zip -r projet1.zip Makefile experiments.sh src

.PHONY: all clean test benchmark save-studsrv plot clean zip
