#!/bin/bash
# Lit le flux STDOUT (d'un benchmark ou du feedback INGInious) depuis STDIN
# et extrait chaque benchmark dans son fichier CSV respectif dans data/.
#
# Utilisation :
#   ./src/part1/benchmark_part1.sh | ./data/save_data.sh
#   ./data/save_data.sh --studsrv < feedback_studsrv.txt
#   ./data/save_data.sh --timestamp [prefix] < feedback.txt
#   ./data/save_data.sh dossier_personnalise < feedback.txt

set -euo pipefail

# Résolution automatique du dossier data/ (même si lancé depuis la racine ou depuis data/)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DATA_DIR="$SCRIPT_DIR"

TIMESTAMP="$(date +'%Y-%m-%d_%H-%M')"

# Gestion des paramètres de destination
case "${1:-}" in
    --studsrv|-s|studsrv)
        OUTPUT_DIR="${BASE_DATA_DIR}/studsrv_${TIMESTAMP}"
        ;;
    --timestamp|-t|timestamp)
        PREFIX="${2:-run}"
        OUTPUT_DIR="${BASE_DATA_DIR}/${PREFIX}_${TIMESTAMP}"
        ;;
    "")
        OUTPUT_DIR="${BASE_DATA_DIR}"
        ;;
    *)
        # Si c'est un chemin absolu ou commençant par data/, l'utiliser directement, sinon le mettre dans data/
        if [[ "$1" = /* ]] || [[ "$1" == data/* ]] || [[ "$1" == */* ]]; then
            OUTPUT_DIR="$1"
        else
            OUTPUT_DIR="${BASE_DATA_DIR}/$1"
        fi
        ;;
esac

mkdir -p "$OUTPUT_DIR"

current_file=""

while IFS= read -r line || [ -n "$line" ]; do
    # Détection des séparateurs "=== nom ===" ou "=== nom.csv ==="
    if [[ "$line" =~ ^===[[:space:]]*([a-zA-Z0-9_-]+)(\.csv)?[[:space:]]*===$ ]]; then
        name="${BASH_REMATCH[1]}"
        current_file="$OUTPUT_DIR/$name.csv"
        > "$current_file"
    elif [ -n "$current_file" ]; then
        echo "$line" >> "$current_file"
    fi
done

echo "Données enregistrées avec succès dans '$OUTPUT_DIR/'."
