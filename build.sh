#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
OUT="$ROOT/server/main.out"

# Compile only the server + shared protocol code.
# Do not include client sources here, because they define their own main().
SRC_FILES=()
while IFS= read -r -d $'\0' file; do
  SRC_FILES+=("$file")
done < <(find "$ROOT/server" "$ROOT/protocol" -type f -name '*.cpp' -print0)

if [ ${#SRC_FILES[@]} -eq 0 ]; then
  echo "No server/protocol sources found to compile" >&2
  exit 1
fi

/usr/bin/clang++ -std=c++17 -g "${SRC_FILES[@]}" -o "$OUT"
