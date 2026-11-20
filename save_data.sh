#!/bin/bash
# Lit le flux STDOUT (d'eval.sh ou du feedback INGInious) depuis STDIN
# et extrait chaque benchmark dans son fichier CSV respectif dans data/.
#
# Utilisation :
#   ./eval.sh | ./save_data.sh
#   ./save_data.sh --studsrv < feedback_studsrv.txt
#   ./save_data.sh --timestamp [prefix] < feedback.txt
#   ./save_data.sh dossier_personnalise < feedback.txt

set -euo pipefail

TIMESTAMP="$(date +'%Y-%m-%d_%H-%M')"

# Gestion des paramètres de destination
case "${1:-}" in
    --studsrv|-s|studsrv)
        OUTPUT_DIR="data/studsrv_${TIMESTAMP}"
        ;;
    --timestamp|-t|timestamp)
        PREFIX="${2:-run}"
        OUTPUT_DIR="data/${PREFIX}_${TIMESTAMP}"
        ;;
    "")
        OUTPUT_DIR="data"
        ;;
    *)
        OUTPUT_DIR="$1"
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
