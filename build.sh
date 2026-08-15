#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
OUT="$ROOT/server/main.out"

# Enable recursive globbing if available
shopt -s globstar 2>/dev/null || true

# Collect all .cpp source files under the project root
SRC_FILES=()
while IFS= read -r -d $'\0' file; do
  SRC_FILES+=("$file")
done < <(find "$ROOT" -type f -name '*.cpp' -print0)

if [ ${#SRC_FILES[@]} -eq 0 ]; then
  echo "No .cpp source files found to compile" >&2
  exit 1
fi

/usr/bin/clang++ -std=c++17 -g "${SRC_FILES[@]}" -o "$OUT"
