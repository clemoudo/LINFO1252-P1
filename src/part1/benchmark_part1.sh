#!/bin/bash

# Arrête le script en cas d'erreur ou d'utilisation d'une variable non définie
set -euo pipefail

# Positionnement à la racine du projet
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$ROOT_DIR"

# --- Configuration et constantes ---
CONFIGS=(2 4 8 16 32)
REPETITIONS=5
TARGET="${1:-all}"

# Localiser le binaire time
if [ -x "/usr/bin/time" ]; then
    TIME_BIN="/usr/bin/time"
elif command -v time >/dev/null 2>&1; then
    TIME_BIN="$(which time)"
else
    echo "Erreur: commande 'time' externe introuvable."
    exit 1
fi

# Compilation préalable (redirection vers STDERR pour garder STDOUT propre)
make -s all >&2

# Fichier temporaire pour récupérer la mesure de time
TMP_TIME=$(mktemp)
trap 'rm -f "$TMP_TIME"' EXIT

# --- Fonction de benchmark pour un programme ---
benchmark() {
    local prog="$1"

    # Balise de début de section pour les scripts de parsing
    echo "=== $prog ==="
    echo "threads,run1,run2,run3,run4,run5"

    for total_threads in "${CONFIGS[@]}"; do
        local line="$total_threads"

        local cmd=()
        if [ "$prog" = "philosophes" ]; then
            cmd=("./bin/$prog" "$total_threads")
        else
            local half=$((total_threads / 2))
            cmd=("./bin/$prog" "$half" "$half")
        fi

        for ((run = 0; run < REPETITIONS; run++)); do
            # Exécution avec redirection de STDOUT et STDERR du binaire vers /dev/null
            $TIME_BIN -f "%e" -o "$TMP_TIME" "${cmd[@]}" >/dev/null 2>&1
            local measure
            measure=$(cat "$TMP_TIME")
            line="$line,$measure"
        done

        echo "$line"
    done
}

# --- Lancement du ou des benchmarks selon l'argument ---
case "$TARGET" in
    philosophes)
        benchmark "philosophes"
        ;;
    producer_consumer|prodcons)
        benchmark "producer_consumer"
        ;;
    reader_writer|read-write)
        benchmark "reader_writer"
        ;;
    all)
        benchmark "philosophes"
        benchmark "producer_consumer"
        benchmark "reader_writer"
        ;;
    *)
        echo "Usage: $0 [philosophes|producer_consumer|reader_writer|all]" >&2
        exit 1
        ;;
esac
