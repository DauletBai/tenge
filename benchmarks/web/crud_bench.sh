#!/usr/bin/env bash
# Simple timing harness for CRUD demo.

set -euo pipefail
BIN="../examples/.bin/crud"

if [[ ! -x "$BIN" ]]; then
  echo "build examples first: make -C examples"
  exit 1
fi

runs=${1:-5}
echo "[bench] crud demo $runs runs"
for i in $(seq 1 "$runs"); do
  /usr/bin/time -lp "$BIN" >/dev/null
done