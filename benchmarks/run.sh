#!/usr/bin/env bash
# Bench harness for Tenge vs C/Rust/Go
# Portable for macOS Bash 3.2 (no associative arrays).
# Build never receives runtime args; only execution does.

set -euo pipefail

# ------------- Config (env-overridable) -------------
REPS="${REPS:-5}"
SIZE="${SIZE:-100000}"

FIB_N="${FIB_N:-90}"
NBODY_N="${NBODY_N:-4096}"
NBODY_STEPS="${NBODY_STEPS:-10}"
NBODY_DT="${NBODY_DT:-0.001}"

RESULTS_DIR="benchmarks/results"
TS="$(date +%Y%m%d_%H%M%S)"
SUITE_CSV="${RESULTS_DIR}/suite_${TS}.csv"
ACC_CSV="${RESULTS_DIR}/var_acc_${TS}.csv"
LATEST_MD="${RESULTS_DIR}/LATEST.md"

say() { echo -e "$*"; }
ensure_results_dir() { mkdir -p "${RESULTS_DIR}"; }

# ------------- Timing helpers -------------
bench_cmd() {
  # Usage: bench_cmd "label" BIN [args...]
  local label="$1"; shift
  local bin="$1"; shift || true

  if [[ ! -x "$bin" ]]; then
    say " -> Skipping $bin (not found)"
    echo "${label},0" >> "${SUITE_CSV}"
    return 0
  fi

  # Run timing in Python for accuracy & average
  local out
  out="$(python3 - "$label" "$bin" "$@" <<'PY'
import sys, os, time, subprocess, statistics, shlex
label = sys.argv[1]
bin_  = sys.argv[2]
args  = sys.argv[3:]
reps  = int(os.environ.get("REPS","5"))
times = []
for _ in range(reps):
    t0 = time.perf_counter_ns()
    try:
        subprocess.run([bin_] + args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except Exception:
        print(f" -> Skipping {shlex.join([bin_] + args)} (failed)")
        print(f"... {label} avg=0 ns")
        print(f"__CSV__:{label}:0")
        sys.exit(0)
    t1 = time.perf_counter_ns()
    times.append(t1 - t0)
avg = int(statistics.mean(times)) if times else 0
print(f"    ... {label} avg={avg} ns")
print(f"__CSV__:{label}:{avg}")
PY
)"
  echo "${out}"
  local csv_line
  csv_line="$(echo "${out}" | awk -F'__CSV__:' '/__CSV__:/ {print $2}' | tr ':' ',')"
  [[ -n "${csv_line:-}" ]] && echo "${csv_line}" >> "${SUITE_CSV}"
}

bench_cmd_acc() {
  bench_cmd "$@"
  local label="$1"
  local last
  last="$(tail -n1 "${SUITE_CSV}" || true)"
  if [[ -n "${last}" && "${last}" == "${label},"* ]]; then
    echo "${last}" >> "${ACC_CSV}"
  fi
}

# ------------- Build phase -------------
ensure_results_dir
: > "${SUITE_CSV}"
: > "${ACC_CSV}"

say "[bench] Running all benchmarks with REPS=${REPS}"
say "[bench] Sort tasks use SIZE=${SIZE}. Fib tasks use fixed N=${FIB_N}."
say "[bench] Results will be saved to ${SUITE_CSV}"

say "[build] compiler -> .bin/tenge"
say "[build] runtime.o built"
make build >/dev/null

# Ensure AOT sort demos (portable, no associative arrays)
ensure_aot_one() {
  # ensure_aot_one SRC_TNG DST_BIN
  local src="$1"
  local dst="$2"
  if [[ -x "${dst}" ]]; then
    return 0
  fi
  if [[ -f "${src}" ]]; then
    local base
    base="$(basename "${src}")"
    say "[aot] ${base%_cli.tng} -> ${dst}.c"
    .bin/tenge -o "${dst}.c" "${src}"
    cc -O3 -march=native -Iinternal/aotminic/runtime \
      "${dst}.c" internal/aotminic/runtime/runtime.c -o "${dst}"
  else
    say "  [skip] ${src} not found"
  fi
}

ensure_tenge_sorts() {
  ensure_aot_one "benchmarks/src/tenge/sort_qsort_cli.tng"  ".bin/sort_cli_qsort"
  ensure_aot_one "benchmarks/src/tenge/sort_msort_cli.tng"  ".bin/sort_cli_msort"
  ensure_aot_one "benchmarks/src/tenge/sort_pdq_cli.tng"    ".bin/sort_cli_pdq"
  ensure_aot_one "benchmarks/src/tenge/sort_radix_cli.tng"  ".bin/sort_cli_radix"
}
ensure_tenge_sorts

# ------------- CSV headers -------------
echo "task,avg_ns" > "${SUITE_CSV}"
echo "task,avg_ns" > "${ACC_CSV}"

# ------------- Suites -------------
say " -> Running sort for tenge/c/rust/go with N=${SIZE}..."
bench_cmd "tenge(qsort)" .bin/sort_cli_qsort "${SIZE}"
bench_cmd "tenge(msort)" .bin/sort_cli_msort "${SIZE}"
bench_cmd "tenge(pdq)"   .bin/sort_cli_pdq   "${SIZE}"
bench_cmd "tenge(radix)" .bin/sort_cli_radix "${SIZE}"
bench_cmd "c(-)"         .bin/sort_c         "${SIZE}"
bench_cmd "rust(-)"      .bin/sort_rs        "${SIZE}"
bench_cmd "go(-)"        .bin/sort_go        "${SIZE}"

say " -> Running fib_iter for tenge/c/rust/go with N=${FIB_N}..."
bench_cmd "tenge"        .bin/fib_cli        "${FIB_N}"
bench_cmd "c"            .bin/fib_iter_c     "${FIB_N}"
bench_cmd "rust"         .bin/fib_iter_rs    "${FIB_N}"
bench_cmd "go"           .bin/fib_iter_go    "${FIB_N}"

say " -> Running fib_rec for tenge/c/rust/go with N=35..."
bench_cmd "tenge"        .bin/fib_rec_cli    35
bench_cmd "c"            .bin/fib_rec_c      35
bench_cmd "rust"         .bin/fib_rec_rs     35
bench_cmd "go"           .bin/fib_rec_go     35

say " -> Running VaR Monte Carlo (sort / ziggurat / qselect) N=1000000, steps=1, alpha=0.99..."
bench_cmd_acc "tenge(sort)" .bin/var_mc_tng_sort 1000000 1 0.99
bench_cmd_acc "tenge(zig)"  .bin/var_mc_tng_zig  1000000 1 0.99
bench_cmd_acc "tenge(qsel)" .bin/var_mc_tng_qsel 1000000 1 0.99
bench_cmd_acc "c(-)"        .bin/var_mc_c       1000000 1 0.99
bench_cmd_acc "rust(-)"     .bin/var_mc_rs      1000000 1 0.99
bench_cmd_acc "go(-)"       .bin/var_mc_go      1000000 1 0.99

say " -> Running nbody (N=${NBODY_N}, steps=${NBODY_STEPS}) in order: tenge, c, rust, go..."
bench_cmd "tenge" .bin/nbody_tng "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "c"     .bin/nbody_c   "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "rust"  .bin/nbody_rs  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "go"    .bin/nbody_go  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"

say " -> Running nbody_sym (symmetric kernel) in order: tenge, c, rust, go..."
bench_cmd "tenge(sym)" .bin/nbody_tng_sym "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "c(sym)"     .bin/nbody_c_sym   "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "rust(sym)"  .bin/nbody_rs_sym  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
bench_cmd "go(sym)"    .bin/nbody_go_sym  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"

say "[bench] All benchmarks finished successfully."
say "[bench] Accuracy CSV: ${ACC_CSV}"

# ------------- LATEST.md -------------
{
  echo "# Latest Benchmark Summary (${TS})"
  echo
  echo "- REPS: ${REPS}"
  echo "- SIZE (sort): ${SIZE}"
  echo "- FIB_N (iter): ${FIB_N}"
  echo "- NBODY: N=${NBODY_N}, steps=${NBODY_STEPS}, dt=${NBODY_DT}"
  echo
  echo "## Results CSV"
  echo "- Suite: \`${SUITE_CSV}\`"
  echo "- VaR Accuracy: \`${ACC_CSV}\`"
  echo
  echo "## Notes"
  echo "- Tenge sort demos are built via AOT only if missing."
  echo "- Runtime arguments are passed **only at execution time**, never at build."
  echo "- All times are averages over REPS using \`time.perf_counter_ns()\`."
} > "${LATEST_MD}"

say "[bench] Summary saved to ${LATEST_MD}"