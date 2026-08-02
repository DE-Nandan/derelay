#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
OUT="$ROOT/server/main.out"
SRC_FILES=(
  "$ROOT/server/main.cpp"
  "$ROOT/server"/**/*.cpp
)
/usr/bin/clang++ -std=c++17 -g "${SRC_FILES[@]}" -o "$OUT"
