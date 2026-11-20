#!/bin/bash

set -euo pipefail

TARGET="${1:-all}"

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

# Lance le benchmark de la partie 1
./src/part1/benchmark_part1.sh "$TARGET"
