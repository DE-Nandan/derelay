#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/tests/test_suite"

/usr/bin/clang++ -std=c++17 -g \
    "$ROOT/tests/TestSuite.cpp" \
    "$ROOT/protocol/Packet.cpp" \
    "$ROOT/protocol/DeliveryPolicy.cpp" \
    "$ROOT/server/network/SessionManager.cpp" \
    -o "$OUT"

"$OUT"
