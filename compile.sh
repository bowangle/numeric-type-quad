#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

# Extra arguments are CMake configuration arguments, e.g.:
# ./compile.sh -DNUMERIC_TYPE_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/install
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build "$BUILD_DIR" --parallel "${BUILD_JOBS:-2}"
