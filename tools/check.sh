#!/usr/bin/env bash
set -euo pipefail

python3 tools/python/format_code.py --check
cmake --preset Debug
cmake --build build/Debug
