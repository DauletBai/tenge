#!/usr/bin/env bash
set -euo pipefail

REPS="${REPS:-5}"
SIZE="${SIZE:-100000}"      # sort size
FIB_N="${FIB_N:-90}"
FIB_REC_N="${FIB_REC_N:-35}"
VAR_N="${VAR_N:-1000000}"
VAR_ALPHA="${VAR_ALPHA:-0.99}"
NBODY_N="${NBODY_N:-4096}"
NBODY_STEPS="${NBODY_STEPS:-10}"
NBODY_DT="${NBODY_DT:-0.001}"

BIN=".bin"
RESULTS_DIR="results"
TS="$(date +%Y%m%d_%H%M%S)"
SUITE_CSV="${RESULTS_DIR}/suite_${TS}.csv"
ACC_CSV="${RESULTS_DIR}/var_acc_${TS}.csv"

mkdir -p "$RESULTS_DIR" "$BIN"

echo "[bench] Running all benchmarks with REPS=${REPS}"
echo "[bench] Sort tasks use SIZE=${SIZE}. Fib tasks use fixed N values."
echo "[bench] Results will be saved to ${SUITE_CSV}"

emit_csv_header() {
  if [ ! -f "$SUITE_CSV" ]; then
    echo "task,lang,variant,N,steps,dt,alpha,avg_ns" > "$SUITE_CSV"
  fi
}

emit_acc_header() {
  if [ ! -f "$ACC_CSV" ]; then
    echo "task,lang,variant,N,alpha,truth_var,truth_es,est_var,est_es,abs_err_var,abs_err_es" > "$ACC_CSV"
  fi
}

run_acc_bench() {
  local label="$1" ; local lang="$2" ; local variant="$3"
  shift 3
  local cmd=("$@")
  if [ ! -x "${cmd[0]}" ]; then
    echo " -> Skipping ${cmd[*]} (not found)"
    return
  fi
  
  echo "    ... Running ${label} accuracy benchmark..."
  local result=$("${cmd[@]}" 2>/dev/null)
  if [ $? -eq 0 ]; then
    echo "    ... ${label} completed: ${result}"
    emit_acc_header
    
    # Parse the improved result format
    local N=$(echo "$result" | grep -o 'N=[0-9]*' | cut -d= -f2)
    local alpha=$(echo "$result" | grep -o 'ALPHA=[0-9.]*' | cut -d= -f2)
    local truth_var=$(echo "$result" | grep -o 'TRUTH_VAR=[-0-9.]*' | cut -d= -f2)
    local truth_es=$(echo "$result" | grep -o 'TRUTH_ES=[-0-9.]*' | cut -d= -f2)
    local est_var=$(echo "$result" | grep -o 'EST_VAR=[-0-9.]*' | cut -d= -f2)
    local est_es=$(echo "$result" | grep -o 'EST_ES=[-0-9.]*' | cut -d= -f2)
    local abs_err_var=$(echo "$result" | grep -o 'ABS_ERR_VAR=[-0-9.]*' | cut -d= -f2)
    local abs_err_es=$(echo "$result" | grep -o 'ABS_ERR_ES=[-0-9.]*' | cut -d= -f2)
    
    echo "${label},${lang},${variant},${N},${alpha},${truth_var},${truth_es},${est_var},${est_es},${abs_err_var},${abs_err_es}" >> "$ACC_CSV"
  else
    echo "    ... ${label} failed"
  fi
}

avg_ns() {
  local cmd="$1"
  local reps="$2"
  local acc=0
  for ((i=0;i<reps;i++)); do
    local t0=$(python3 - <<'PY'
import time; print(time.time_ns())
PY
)
    $cmd >/dev/null
    local t1=$(python3 - <<'PY'
import time; print(time.time_ns())
PY
)
    acc=$((acc + (t1 - t0)))
  done
  echo $((acc / reps))
}

avg_tenge_ns() {
  local cmd="$1"
  local reps="$2"
  local acc=0
  for ((i=0;i<reps;i++)); do
    local output=$($cmd 2>/dev/null)
    local time_ns=$(echo "$output" | grep -o "TIME_NS=[0-9]*" | cut -d= -f2)
    if [ -n "$time_ns" ]; then
      acc=$((acc + time_ns))
    fi
  done
  echo $((acc / reps))
}

run_bin_avg() {
  local label="$1" ; local lang="$2" ; local variant="$3"
  shift 3
  local cmd=("$@")
  if [ ! -x "${cmd[0]}" ]; then
    echo " -> Skipping ${cmd[*]} (not found)"
    return
  fi
  
  # Special handling for Tenge AOT demos that output TIME_NS
  if [[ "$lang" == "tenge" && "${cmd[0]}" == *"fib_cli"* ]]; then
    local avg=$(avg_tenge_ns "${cmd[*]}" "$REPS")
  elif [[ "$lang" == "tenge" && "${cmd[0]}" == *"fib_rec_cli"* ]]; then
    local avg=$(avg_tenge_ns "${cmd[*]}" "$REPS")
  else
    local avg=$(avg_ns "${cmd[*]}" "$REPS")
  fi
  
  echo "    ... ${label} avg=${avg} ns"
  emit_csv_header
  # Heuristics to fill columns
  local task="${label%% *}"
  local N="" steps="" dt="" alpha=""
  case "$task" in
    sort*) N="$SIZE" ;;
    fib_iter) N="$FIB_N" ;;
    fib_rec) N="$FIB_REC_N" ;;
    var_mc*) N="$VAR_N"; alpha="$VAR_ALPHA" ;;
    nbody) N="$NBODY_N"; steps="$NBODY_STEPS"; dt="$NBODY_DT" ;;
    nbody_sym) N="$NBODY_N"; steps="$NBODY_STEPS"; dt="$NBODY_DT" ;;
  esac
  echo "${task},${lang},${variant},${N},${steps},${dt},${alpha},${avg}" >> "$SUITE_CSV"
}

# -------------------------
# Build on demand (safeguard)
# -------------------------
build_all() {
  make build
}

# -------------------------
# Bench sequences
# -------------------------
# build_all  # Already built in main directory

# sort
echo " -> Running sort for tenge/c/rust/go with N=${SIZE}..."
run_bin_avg "sort"       "tenge" ""         "${BIN}/sort_tenge"      "${SIZE}"
run_bin_avg "sort"       "c"     ""         "${BIN}/sort_c"         "${SIZE}"
run_bin_avg "sort"       "rust"  ""         "${BIN}/sort_rs"        "${SIZE}"
run_bin_avg "sort"       "go"    ""         "${BIN}/sort_go"        "${SIZE}"

# fib - all with same methodology as Tenge
echo " -> Running fib_iter for tenge/c/rust/go with N=${FIB_N} (same methodology)..."
run_bin_avg "fib_iter" "tenge" "" "${BIN}/fib_cli"     "${FIB_N}" "2000000"
run_bin_avg "fib_iter" "c"     "" "${BIN}/fib_iter_c"  "${FIB_N}" "2000000"
run_bin_avg "fib_iter" "rust"  "" "${BIN}/fib_iter_rs" "${FIB_N}" "2000000"
run_bin_avg "fib_iter" "go"    "" "${BIN}/fib_iter_go" "${FIB_N}" "2000000"

# fib_fixed - with same methodology as Tenge
echo " -> Running fib_iter_fixed for c/rust/go with N=${FIB_N} (same methodology as Tenge)..."
run_bin_avg "fib_iter_fixed" "c"     "" "${BIN}/fib_iter_c_fixed"  "${FIB_N}" "2000000"
run_bin_avg "fib_iter_fixed" "rust"  "" "${BIN}/fib_iter_rs_fixed" "${FIB_N}" "2000000"
run_bin_avg "fib_iter_fixed" "go"    "" "${BIN}/fib_iter_go_fixed" "${FIB_N}" "2000000"

echo " -> Running fib_rec for tenge/c/rust/go with N=${FIB_REC_N}..."
run_bin_avg "fib_rec" "tenge" "" "${BIN}/fib_rec_cli"  "${FIB_REC_N}" "2000"
run_bin_avg "fib_rec" "c"     "" "${BIN}/fib_rec_c"    "${FIB_REC_N}"
run_bin_avg "fib_rec" "rust"  "" "${BIN}/fib_rec_rs"   "${FIB_REC_N}"
run_bin_avg "fib_rec" "go"    "" "${BIN}/fib_rec_go"   "${FIB_REC_N}"

# VaR Monte Carlo
echo " -> Running VaR Monte Carlo (sort / ziggurat / qselect) N=${VAR_N}, steps=1, alpha=${VAR_ALPHA}..."
run_bin_avg "var_mc_sort" "tenge" "sort"   "${BIN}/var_mc_tng_sort" "${VAR_N}" "1" "${VAR_ALPHA}"
run_bin_avg "var_mc_zig"  "tenge" "zig"    "${BIN}/var_mc_tng_zig"  "${VAR_N}" "1" "${VAR_ALPHA}"
run_bin_avg "var_mc_qsel" "tenge" "qsel"   "${BIN}/var_mc_tng_qsel" "${VAR_N}" "1" "${VAR_ALPHA}"
run_bin_avg "var_mc"      "c"     ""       "${BIN}/var_mc_c"        "${VAR_N}" "1" "${VAR_ALPHA}"
run_bin_avg "var_mc"      "rust"  ""       "${BIN}/var_mc_rs"       "${VAR_N}" "1" "${VAR_ALPHA}"
run_bin_avg "var_mc"      "go"    ""       "${BIN}/var_mc_go"       "${VAR_N}" "1" "${VAR_ALPHA}"

# VaR Monte Carlo Accuracy
echo " -> Running VaR Monte Carlo Accuracy N=${VAR_N}, alpha=${VAR_ALPHA}..."
run_acc_bench "var_mc_acc"  "c"     ""       "${BIN}/var_mc_c_acc_improved"    "${VAR_N}" "0.0" "1.0" "${VAR_ALPHA}"
run_acc_bench "var_mc_acc"  "rust"  ""       "${BIN}/var_mc_rs_acc_improved"   "${VAR_N}" "0.0" "1.0" "${VAR_ALPHA}"
run_acc_bench "var_mc_acc"  "go"    ""       "${BIN}/var_mc_go_acc_improved"   "${VAR_N}" "0.0" "1.0" "${VAR_ALPHA}"
run_acc_bench "var_mc_acc"  "tenge" ""       "${BIN}/var_mc_c_acc_tenge"  "${VAR_N}" "0.0" "1.0" "${VAR_ALPHA}"

# N-body
echo " -> Running nbody (N=${NBODY_N}, steps=${NBODY_STEPS}) in order: tenge, c, rust, go..."
run_bin_avg "nbody" "tenge" "" "${BIN}/nbody_tng" "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody" "c"     "" "${BIN}/nbody_c"   "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody" "rust"  "" "${BIN}/nbody_rs"  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody" "go"    "" "${BIN}/nbody_go"  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"

echo " -> Running nbody_sym (symmetric kernel) in order: tenge, c, rust, go..."
run_bin_avg "nbody_sym" "tenge" "" "${BIN}/nbody_tng_sym" "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody_sym" "c"     "" "${BIN}/nbody_c_sym"   "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody_sym" "rust"  "" "${BIN}/nbody_rs_sym"  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"
run_bin_avg "nbody_sym" "go"    "" "${BIN}/nbody_go_sym"  "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}"

echo "[bench] All benchmarks finished successfully."
echo "[bench] Accuracy CSV: ${ACC_CSV}"