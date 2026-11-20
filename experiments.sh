#!/bin/bash

set -euo pipefail

TARGET="${1:-all}"

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

./eval.sh "$TARGET"
