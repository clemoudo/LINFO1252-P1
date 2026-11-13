#!/bin/bash

# Arrête le script en cas d'erreur ou d'utilisation d'une variable non définie
set -euo pipefail

# --- Configuration et constantes ---
MAX_THREADS=8
REPETITIONS=5
TIME_BIN="/usr/bin/time"

# Fichier temporaire pour récupérer la sortie de time sans écraser STDERR de make
TMP_TIME=$(mktemp)
trap 'rm -f "$TMP_TIME"' EXIT

# --- En-tête du tableau CSV sur STDOUT ---
echo "threads,run1,run2,run3,run4,run5"

for ((threads = 1; threads <= MAX_THREADS; threads *= 2)); do
    line="$threads"

    for ((run = 1; run <= REPETITIONS; run++)); do
        make -s clean >/dev/null 2>&1
        $TIME_BIN -f "%e" -o "$TMP_TIME" make -s -j "$threads" 2>/dev/null
        measure=$(cat "$TMP_TIME")
        line="$line,$measure"
    done

    echo "$line"
done
