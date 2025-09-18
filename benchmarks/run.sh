#!/usr/bin/env bash
# FILE: benchmarks/run.sh
# Purpose: Full benchmark suite (fib, sort, VaR MC, N-body). Robust time parsing + external fallback timer.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="${ROOT}/.bin"
RES="${ROOT}/benchmarks/results"
mkdir -p "$BIN" "$RES"

REPS="${REPS:-5}"
SIZE_SORT="${SIZE_SORT:-100000}"

FIB_N_ITER="${FIB_N_ITER:-90}"
FIB_N_REC="${FIB_N_REC:-35}"

VAR_N="${VAR_N:-1000000}"
VAR_STEPS="${VAR_STEPS:-1}"
VAR_ALPHA="${VAR_ALPHA:-0.99}"

NBODY_N="${NBODY_N:-4096}"
NBODY_STEPS="${NBODY_STEPS:-10}"
NBODY_DT="${NBODY_DT:-0.001}"

SUITE="${RES}/suite_$(date +%Y%m%d_%H%M%S).csv"
echo "ts,task,impl,paramN,time_ns,extra" > "$SUITE"

log(){ echo "$@" 1>&2; }
append(){ printf "%s,%s,%s,%s,%s,%s\n" "$(date +%s)" "$1" "$2" "$3" "$4" "$5" >> "$SUITE"; }

have_dir(){ [[ -d "$1" ]]; }
have_file(){ [[ -f "$1" && -x "$1" ]]; }

extract_ns(){
  local text="$1"
  local ns
  ns="$(printf "%s" "$text" | sed -n 's/.*TIME_NS=\([0-9][0-9]*\).*/\1/p' | head -n1 || true)"
  if [[ -n "${ns:-}" ]]; then echo "$ns"; return; fi
  local us
  us="$(printf "%s" "$text" | sed -n 's/.*TIME_US=\([0-9][0-9]*\).*/\1/p' | head -n1 || true)"
  if [[ -n "${us:-}" ]]; then echo $((us*1000)); return; fi
  echo ""
}

extract_extra(){
  local text="$1"
  local ex
  ex="$(printf "%s" "$text" | sed -n 's/.*\(ENERGY=[-0-9.][-0-9.]*\).*/\1/p' | head -n1 || true)"
  echo "${ex:-}"
}

# External wall-clock fallback in ns using Python's perf_counter_ns (no micro-batching).
wall_ns(){
  python3 - <<'PY' "$@"
import sys, subprocess, time
cmd = sys.argv[1:]
t0 = time.perf_counter_ns()
try:
    subprocess.run(cmd, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
except Exception:
    pass
t1 = time.perf_counter_ns()
print(t1 - t0)
PY
}

avg(){
  local exe="$1" task="$2" impl="$3" nparam="$4" args="$5"
  local acc=0 ok=0 last_extra=""

  for ((r=1; r<=REPS; r++)); do
    if ! have_file "$exe"; then
      append "$task" "$impl" "$nparam" "0" ""
      continue
    fi
    local out tns extra
    out="$("$exe" $args 2>&1 || true)"   # capture both streams to avoid losing TIME_* on stderr
    tns="$(extract_ns "$out")"
    extra="$(extract_extra "$out")"
    if [[ -z "${tns:-}" || "$tns" == "0" ]]; then
      tns="$(wall_ns "$exe" $args)"
      [[ -z "${tns:-}" ]] && tns=0
    fi
    append "$task" "$impl" "$nparam" "${tns:-0}" "$extra"
    acc=$((acc + tns)); ok=$((ok + 1))
    last_extra="$extra"
  done

  if ((ok>0)); then echo $((acc/ok)); else echo 0; fi
}

echo "[bench] Running all benchmarks with REPS=${REPS}"
echo "[bench] Sort tasks use SIZE=${SIZE_SORT}. Fib tasks use fixed N values."
echo "[bench] Results will be saved to ${SUITE}"

# -------- Build --------
log "[build] compiler -> .bin/tenge"
go build -o "${BIN}/tenge" ./cmd/tenge

# C
log "[build] c_sort -> .bin/sort_c"
cc -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math benchmarks/src/c/sort.c -o "${BIN}/sort_c" || true
log "[build] c_fib_iter -> .bin/fib_iter_c"
cc -O3 -march=native benchmarks/src/c/fib_iter.c -o "${BIN}/fib_iter_c" || true
log "[build] c_fib_rec -> .bin/fib_rec_c"
cc -O3 -march=native benchmarks/src/c/fib_rec.c -o "${BIN}/fib_rec_c" || true
log "[build] c_var_mc -> .bin/var_mc_c"
cc -O3 -march=native benchmarks/src/c/var_monte_carlo.c -o "${BIN}/var_mc_c" || true
log "[build] c_nbody -> .bin/nbody_c"
cc -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math benchmarks/src/c/nbody.c -o "${BIN}/nbody_c" || true
log "[build] c_nbody_sym -> .bin/nbody_c_sym"
cc -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math benchmarks/src/c/nbody_sym.c -o "${BIN}/nbody_c_sym" || true

# Go (guard)
log "[build] go_sort -> .bin/sort_go"
if have_dir benchmarks/src/go/sort; then (cd benchmarks/src/go/sort && GOMAXPROCS=1 go build -o "${BIN}/sort_go"); else log "  [skip] go/sort not found"; fi
log "[build] go_fib_iter -> .bin/fib_iter_go"
if have_dir benchmarks/src/go/fib_iter; then (cd benchmarks/src/go/fib_iter && GOMAXPROCS=1 go build -o "${BIN}/fib_iter_go"); else log "  [skip] go/fib_iter not found"; fi
log "[build] go_fib_rec -> .bin/fib_rec_go"
if have_dir benchmarks/src/go/fib_rec; then (cd benchmarks/src/go/fib_rec && GOMAXPROCS=1 go build -o "${BIN}/fib_rec_go"); else log "  [skip] go/fib_rec not found"; fi
log "[build] go_var_mc -> .bin/var_mc_go"
if have_dir benchmarks/src/go/var_mc; then (cd benchmarks/src/go/var_mc && GOMAXPROCS=1 go build -o "${BIN}/var_mc_go"); else log "  [skip] go/var_mc not found"; fi
log "[build] go_nbody -> .bin/nbody_go"
if have_dir benchmarks/src/go/nbody; then (cd benchmarks/src/go/nbody && GOMAXPROCS=1 go build -o "${BIN}/nbody_go"); else log "  [skip] go/nbody not found"; fi
log "[build] go_nbody_sym -> .bin/nbody_go_sym"
if have_dir benchmarks/src/go/nbody_sym; then (cd benchmarks/src/go/nbody_sym && GOMAXPROCS=1 go build -o "${BIN}/nbody_go_sym"); else log "  [skip] go/nbody_sym not found"; fi

# Rust (guard)
log "[build] rust_sort -> .bin/sort_rs"
if have_dir benchmarks/src/rust/sort; then
  (cd benchmarks/src/rust/sort && cargo build --release && cp -f target/release/sort "${BIN}/sort_rs") || log "  [skip] rust/sort not built"
else log "  [skip] rust/sort not found"; fi
log "[build] rust_fib_iter -> .bin/fib_iter_rs"
if have_dir benchmarks/src/rust/fib_iter; then
  (cd benchmarks/src/rust/fib_iter && cargo build --release && cp -f target/release/fib_iter "${BIN}/fib_iter_rs") || log "  [skip] rust/fib_iter not built"
else log "  [skip] rust/fib_iter not found"; fi
log "[build] rust_fib_rec -> .bin/fib_rec_rs"
if have_dir benchmarks/src/rust/fib_rec; then
  (cd benchmarks/src/rust/fib_rec && cargo build --release && cp -f target/release/fib_rec "${BIN}/fib_rec_rs") || log "  [skip] rust/fib_rec not built"
else log "  [skip] rust/fib_rec not found"; fi
log "[build] rust_var_mc -> .bin/var_mc_rs"
if have_dir benchmarks/src/rust/var_mc; then
  (cd benchmarks/src/rust/var_mc && cargo build --release && cp -f target/release/var_mc "${BIN}/var_mc_rs") || log "  [skip] rust/var_mc not built"
else log "  [skip] rust/var_mc not found"; fi
log "[build] rust_nbody -> .bin/nbody_rs"
if have_dir benchmarks/src/rust/nbody; then
  (cd benchmarks/src/rust/nbody && cargo build --release && cp -f target/release/nbody "${BIN}/nbody_rs") || log "  [skip] rust/nbody not built"
else log "  [skip] rust/nbody not found"; fi
log "[build] rust_nbody_sym -> .bin/nbody_rs_sym"
if have_dir benchmarks/src/rust/nbody_sym; then
  (cd benchmarks/src/rust/nbody_sym && cargo build --release && cp -f target/release/nbody_sym "${BIN}/nbody_rs_sym") || log "  [skip] rust/nbody_sym not built"
else log "  [skip] rust/nbody_sym not found"; fi

# Tenge AOT emit + compile
emit(){ "${BIN}/tenge" -o "$1" "$2"; }

log "[aot] fib_iter -> .bin/fib_cli";          emit "${BIN}/fib_cli.c"          benchmarks/src/tenge/fib_iter_cli.tng          && cc -O3 -march=native "${BIN}/fib_cli.c" -o "${BIN}/fib_cli" || true
log "[aot] fib_rec  -> .bin/fib_rec_cli";      emit "${BIN}/fib_rec_cli.c"      benchmarks/src/tenge/fib_rec_cli.tng           && cc -O3 -march=native "${BIN}/fib_rec_cli.c" -o "${BIN}/fib_rec_cli" || true
log "[aot] sort_qsort -> .bin/sort_cli_qsort"; emit "${BIN}/sort_cli_qsort.c"   benchmarks/src/tenge/sort_cli_qsort.tng        && cc -O3 -march=native "${BIN}/sort_cli_qsort.c" -o "${BIN}/sort_cli_qsort" || true
log "[aot] sort_msort -> .bin/sort_cli_msort"; emit "${BIN}/sort_cli_msort.c"   benchmarks/src/tenge/sort_cli_msort.tng        && cc -O3 -march=native "${BIN}/sort_cli_msort.c" -o "${BIN}/sort_cli_msort" || true
log "[aot] sort_pdq -> .bin/sort_cli_pdq";     emit "${BIN}/sort_cli_pdq.c"     benchmarks/src/tenge/sort_cli_pdq.tng          && cc -O3 -march=native "${BIN}/sort_cli_pdq.c" -o "${BIN}/sort_cli_pdq" || true
log "[aot] sort_radix -> .bin/sort_cli_radix"; emit "${BIN}/sort_cli_radix.c"   benchmarks/src/tenge/sort_cli_radix.tng        && cc -O3 -march=native "${BIN}/sort_cli_radix.c" -o "${BIN}/sort_cli_radix" || true
log "[aot] var_mc_sort -> .bin/var_mc_tng_sort"; emit "${BIN}/var_mc_tng_sort.c" benchmarks/src/tenge/var_mc_sort_cli.tng       && cc -O3 -march=native "${BIN}/var_mc_tng_sort.c" -o "${BIN}/var_mc_tng_sort" || true
log "[aot] var_mc_zig  -> .bin/var_mc_tng_zig";  emit "${BIN}/var_mc_tng_zig.c"  benchmarks/src/tenge/var_mc_zig_cli.tng        && cc -O3 -march=native "${BIN}/var_mc_tng_zig.c"  -o "${BIN}/var_mc_tng_zig"  || true
log "[aot] var_mc_qsel -> .bin/var_mc_tng_qsel"; emit "${BIN}/var_mc_tng_qsel.c" benchmarks/src/tenge/var_mc_qsel_cli.tng       && cc -O3 -march=native "${BIN}/var_mc_tng_qsel.c" -o "${BIN}/var_mc_tng_qsel" || true
log "[aot] nbody      -> .bin/nbody_tng";       emit "${BIN}/nbody_tng.c"       benchmarks/src/tenge/nbody_cli.tng             && cc -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math "${BIN}/nbody_tng.c" -o "${BIN}/nbody_tng" || true
log "[aot] nbody_sym  -> .bin/nbody_tng_sym";   emit "${BIN}/nbody_tng_sym.c"   benchmarks/src/tenge/nbody_sym_cli.tng         && cc -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math "${BIN}/nbody_tng_sym.c" -o "${BIN}/nbody_tng_sym" || true

log "[build] All targets built."

# -------- Run blocks (Tenge → C → Rust → Go) --------

echo " -> Running sort for tenge/c/rust/go with N=${SIZE_SORT}..."
printf "    ... Tenge(qsort) avg=%s ns\n" "$(avg "${BIN}/sort_cli_qsort" "sort" "tenge" "${SIZE_SORT}" "${SIZE_SORT}")"
printf "    ... C(qsort)     avg=%s ns\n" "$(avg "${BIN}/sort_c"        "sort" "c"     "${SIZE_SORT}" "${SIZE_SORT}")"
if have_file "${BIN}/sort_rs"; then printf "    ... Rust sort    avg=%s ns\n" "$(avg "${BIN}/sort_rs" "sort" "rust" "${SIZE_SORT}" "${SIZE_SORT}")"; else echo "    ... Skipping Rust sort"; fi
if have_file "${BIN}/sort_go"; then printf "    ... Go sort      avg=%s ns\n"   "$(avg "${BIN}/sort_go" "sort" "go"   "${SIZE_SORT}" "${SIZE_SORT}")";   else echo "    ... Skipping Go sort"; fi

echo " -> Running fib_iter for tenge/c/rust/go with N=${FIB_N_ITER}..."
printf "    ... Tenge avg=%s ns\n" "$(avg "${BIN}/fib_cli"     "fib_iter" "tenge" "${FIB_N_ITER}" "${FIB_N_ITER}")"
printf "    ... C     avg=%s ns\n" "$(avg "${BIN}/fib_iter_c" "fib_iter" "c"     "${FIB_N_ITER}" "${FIB_N_ITER}")"
if have_file "${BIN}/fib_iter_rs"; then printf "    ... Rust avg=%s ns\n" "$(avg "${BIN}/fib_iter_rs" "fib_iter" "rust" "${FIB_N_ITER}" "${FIB_N_ITER}")"; else echo "    ... Skipping Rust fib_iter"; fi
if have_file "${BIN}/fib_iter_go"; then printf "    ... Go   avg=%s ns\n" "$(avg "${BIN}/fib_iter_go" "fib_iter" "go"   "${FIB_N_ITER}" "${FIB_N_ITER}")"; else echo "    ... Skipping Go fib_iter"; fi

echo " -> Running fib_rec for tenge/c/rust/go with N=${FIB_N_REC}..."
printf "    ... Tenge avg=%s ns\n" "$(avg "${BIN}/fib_rec_cli" "fib_rec" "tenge" "${FIB_N_REC}" "${FIB_N_REC}")"
printf "    ... C     avg=%s ns\n" "$(avg "${BIN}/fib_rec_c"   "fib_rec" "c"     "${FIB_N_REC}" "${FIB_N_REC}")"
if have_file "${BIN}/fib_rec_rs"; then printf "    ... Rust avg=%s ns\n" "$(avg "${BIN}/fib_rec_rs"  "fib_rec" "rust"  "${FIB_N_REC}" "${FIB_N_REC}")"; else echo "    ... Skipping Rust fib_rec"; fi
if have_file "${BIN}/fib_rec_go"; then printf "    ... Go   avg=%s ns\n" "$(avg "${BIN}/fib_rec_go"  "fib_rec" "go"    "${FIB_N_REC}" "${FIB_N_REC}")"; else echo "    ... Skipping Go fib_rec"; fi

echo " -> Running VaR Monte Carlo (sort / ziggurat / qselect) N=${VAR_N}, steps=${VAR_STEPS}, alpha=${VAR_ALPHA}..."
printf "    ... Tenge(sort)  avg=%s ns\n" "$(avg "${BIN}/var_mc_tng_sort" "var_mc_sort" "tenge" "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"
printf "    ... Tenge(zig)   avg=%s ns\n" "$(avg "${BIN}/var_mc_tng_zig"  "var_mc_zig"  "tenge" "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"
printf "    ... Tenge(qsel)  avg=%s ns\n" "$(avg "${BIN}/var_mc_tng_qsel" "var_mc_qsel" "tenge" "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"
printf "    ... C            avg=%s ns\n" "$(avg "${BIN}/var_mc_c"        "var_mc"      "c"     "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"
if have_file "${BIN}/var_mc_rs"; then printf "    ... Rust         avg=%s ns\n" "$(avg "${BIN}/var_mc_rs" "var_mc" "rust" "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"; else echo "    ... Skipping Rust var_mc"; fi
if have_file "${BIN}/var_mc_go"; then printf "    ... Go           avg=%s ns\n" "$(avg "${BIN}/var_mc_go" "var_mc" "go"   "${VAR_N}" "${VAR_N} ${VAR_STEPS} ${VAR_ALPHA}")"; else echo "    ... Skipping Go var_mc"; fi

echo " -> Running nbody (N=${NBODY_N}, steps=${NBODY_STEPS}) in order: tenge, c, rust, go..."
printf "    ... Tenge avg=%s ns\n" "$(avg "${BIN}/nbody_tng" "nbody" "tenge" "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"
printf "    ... C     avg=%s ns\n" "$(avg "${BIN}/nbody_c"   "nbody" "c"     "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"
if have_file "${BIN}/nbody_rs"; then printf "    ... Rust  avg=%s ns\n" "$(avg "${BIN}/nbody_rs"  "nbody" "rust"  "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"; else echo "    ... Skipping Rust nbody"; fi
if have_file "${BIN}/nbody_go"; then printf "    ... Go    avg=%s ns\n" "$(avg "${BIN}/nbody_go"  "nbody" "go"    "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"; else echo "    ... Skipping Go nbody"; fi

echo " -> Running nbody_sym (symmetric kernel) in order: tenge, c, rust, go..."
printf "    ... Tenge(sym) avg=%s ns\n" "$(avg "${BIN}/nbody_tng_sym" "nbody_sym" "tenge" "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"
printf "    ... C(sym)     avg=%s ns\n" "$(avg "${BIN}/nbody_c_sym"   "nbody_sym" "c"     "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"
if have_file "${BIN}/nbody_rs_sym"; then printf "    ... Rust(sym)  avg=%s ns\n" "$(avg "${BIN}/nbody_rs_sym"  "nbody_sym" "rust"  "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"; else echo "    ... Skipping Rust nbody_sym"; fi
if have_file "${BIN}/nbody_go_sym"; then printf "    ... Go(sym)    avg=%s ns\n" "$(avg "${BIN}/nbody_go_sym"  "nbody_sym" "go"    "${NBODY_N}" "${NBODY_N} ${NBODY_STEPS} ${NBODY_DT}")"; else echo "    ... Skipping Go nbody_sym"; fi

echo "[bench] All benchmarks finished successfully."