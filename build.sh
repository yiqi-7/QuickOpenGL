#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
TARGET="${1:-}"
BUILD_DIR="$ROOT/build"
cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

if [[ -n "$TARGET" ]]; then
  cmake --build "$BUILD_DIR" --target "$TARGET" -j
else
  cmake --build "$BUILD_DIR" -j
fi
if [[ "${2:-}" == "--run" ]]; then
  if [[ -z "$TARGET" ]]; then echo "Usage: ./build.sh TARGET --run"; exit 2; fi
  if [[ -x "$BUILD_DIR/$TARGET" ]]; then
    (cd "$ROOT" && "$BUILD_DIR/$TARGET")
  elif [[ -x "$BUILD_DIR/Release/$TARGET" ]]; then
    (cd "$ROOT" && "$BUILD_DIR/Release/$TARGET")
  else
    echo "Executable not found for target: $TARGET" >&2
    exit 1
  fi
fi
