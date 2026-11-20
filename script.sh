#!/bin/bash

# Arrête le script en cas d'erreur ou d'utilisation d'une variable non définie
set -euo pipefail

# --- Configuration et constantes ---
CONFIGS=(2 4 8 16 32)
REPETITIONS=5
TIME_BIN="/usr/bin/time"
OUTPUT_DIR="data"

# Vérification de l'existence de /usr/bin/time
if [ ! -x "$TIME_BIN" ]; then
    echo "Erreur: $TIME_BIN est introuvable."
    exit 1
fi

# Compilation préalable des binaires
make -s all

# Création du dossier pour les fichiers CSV
mkdir -p "$OUTPUT_DIR"

# Fichier temporaire pour récupérer la mesure de time
TMP_TIME=$(mktemp)
trap 'rm -f "$TMP_TIME"' EXIT

# --- Fonction de benchmark pour un programme ---
benchmark() {
    local prog="$1"
    local output_csv="$OUTPUT_DIR/$prog.csv"

    echo "threads,run1,run2,run3,run4,run5" > "$output_csv"

    for total_threads in "${CONFIGS[@]}"; do
        local line="$total_threads"

        # Préparation de la commande selon le type de programme
        local cmd=()
        if [ "$prog" = "philosophes" ]; then
            cmd=("./bin/$prog" "$total_threads")
        else
            local half=$((total_threads / 2))
            cmd=("./bin/$prog" "$half" "$half")
        fi

        for ((run = 0; run < REPETITIONS; run++)); do
            # Exécution avec redirection de STDOUT et STDERR vers /dev/null
            $TIME_BIN -f "%e" -o "$TMP_TIME" "${cmd[@]}" >/dev/null 2>&1
            local measure
            measure=$(cat "$TMP_TIME")
            line="$line,$measure"
        done

        echo "$line" >> "$output_csv"
    done
}

# --- Lancement des benchmarks pour les 3 programmes ---
benchmark "philosophes"
benchmark "prodcons"
benchmark "read-write"
