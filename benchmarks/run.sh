#!/usr/bin/env bash
set -euo pipefail

# ---------- Конфиг по умолчанию ----------
: "${REPS:=5}"
: "${WARMUP:=2}"                # Сколько тёплых прогонов перед замером
: "${PROFILE:=strict}"          # MR-2 будет использовать; сейчас не влияет на сборку
: "${SIZE:=100000}"
: "${FIB_N:=90}"
: "${NBODY_N:=4096}"
: "${NBODY_STEPS:=10}"
: "${NBODY_DT:=0.001}"
: "${INNER_REPS:=0}"            # 0 => отключено (как раньше); >0 => внутр. мультипликатор

mkdir -p benchmarks/results

ts_now() { # наносекунды. На macOS date +%s%N может не работать — используем python.
  python3 - <<'PY'
import time,sys
print(int(time.time_ns()))
PY
}

dur_ns() { # ns = t2 - t1
  python3 - "$1" "$2" <<'PY'
import sys
t1=int(sys.argv[1]); t2=int(sys.argv[2])
print(t2-t1)
PY
}

avg_ns() { # среднее по списку ns
  python3 - "$@" <<'PY'
import sys
xs=list(map(int,sys.argv[1:]))
print(sum(xs)//max(1,len(xs)))
PY
}

bench_cmd() {
  # bench_cmd "<display_name>" <cmd...>
  local name="$1"; shift
  local -a times=()
  local warm="$WARMUP"; local reps="$REPS"

  # WARMUP (результаты игнорируем)
  for ((i=0;i<warm;i++)); do
    "$@" >/dev/null 2>&1 || true
  done

  # MEASURE
  for ((i=0;i<reps;i++)); do
    local t1 t2
    t1="$(ts_now)"
    "$@" >/dev/null 2>&1
    t2="$(ts_now)"
    times+=("$(dur_ns "$t1" "$t2")")
  done
  local a="$(avg_ns "${times[@]}")"
  printf "%-12s avg=%s ns\n" "$name" "$a"
  echo "__CSV__:${name}:${a}"
}

fatal_missing() {
  echo "FATAL: required binary not found: $1" >&2
  exit 1
}

# ---------- Проверяем необходимые бинарники ----------
need_bins=(
  ".bin/sort_cli_qsort" ".bin/sort_cli_msort" ".bin/sort_cli_pdq" ".bin/sort_cli_radix"
  ".bin/sort_c" ".bin/sort_rs" ".bin/sort"
  ".bin/fib_cli" ".bin/fib_iter_c" ".bin/fib_iter_rs" ".bin/fib_iter"
  ".bin/fib_rec_cli" ".bin/fib_rec_c" ".bin/fib_rec_rs" ".bin/fib_rec"
  ".bin/var_monte_carlo_c" ".bin/var_mc_rs" ".bin/var_mc"
  ".bin/var_mc_tng_sort" ".bin/var_mc_tng_zig" ".bin/var_mc_tng_qsel"
  ".bin/nbody_tng" ".bin/nbody_c" ".bin/nbody_rs" ".bin/nbody"
  ".bin/nbody_tng_sym" ".bin/nbody_sym_c" ".bin/nbody_rs_sym" ".bin/nbody_sym"
)
for b in "${need_bins[@]}"; do
  [[ -x "$b" ]] || fatal_missing "$b"
done

stamp="$(date +%Y%m%d_%H%M%S)"
suite_csv="benchmarks/results/suite_${stamp}.csv"
acc_csv="benchmarks/results/var_acc_${stamp}.csv"
printf "" >"$suite_csv"
printf "" >"$acc_csv"

echo "[bench] Running all benchmarks with REPS=${REPS}, WARMUP=${WARMUP}, INNER_REPS=${INNER_REPS}"
echo "[bench] Sort SIZE=${SIZE}, Fib N=${FIB_N}, N-body N=${NBODY_N}"

# ---------- SORT ----------
echo "sort         tenge(qsort) $(bench_cmd "tenge(qsort)" .bin/sort_cli_qsort "${SIZE}")" | tee -a "$suite_csv"
echo "sort         tenge(msort) $(bench_cmd "tenge(msort)" .bin/sort_cli_msort "${SIZE}")" | tee -a "$suite_csv"
echo "sort         tenge(pdq)   $(bench_cmd "tenge(pdq)"   .bin/sort_cli_pdq   "${SIZE}")" | tee -a "$suite_csv"
echo "sort         tenge(radix) $(bench_cmd "tenge(radix)" .bin/sort_cli_radix "${SIZE}")" | tee -a "$suite_csv"
echo "sort         c            $(bench_cmd "c(-)"         .bin/sort_c         "${SIZE}")" | tee -a "$suite_csv"
echo "sort         rust         $(bench_cmd "rust(-)"      .bin/sort_rs        "${SIZE}")" | tee -a "$suite_csv"
echo "sort         go           $(bench_cmd "go(-)"        .bin/sort           "${SIZE}")" | tee -a "$suite_csv"

# ---------- FIB ITER (все языки) ----------
# Передаём INNER_REPS в окружении; бинарники не печатают в горячем участке.
echo "fib_iter     tenge        $(INNER_REPS=${INNER_REPS} bench_cmd "tenge" .bin/fib_cli      "${FIB_N}")"   | tee -a "$suite_csv"
echo "fib_iter     c            $(INNER_REPS=${INNER_REPS} bench_cmd "c"     .bin/fib_iter_c   "${FIB_N}")"   | tee -a "$suite_csv"
echo "fib_iter     rust         $(INNER_REPS=${INNER_REPS} bench_cmd "rust"  .bin/fib_iter_rs  "${FIB_N}")"   | tee -a "$suite_csv"
echo "fib_iter     go           $(INNER_REPS=${INNER_REPS} bench_cmd "go"    .bin/fib_iter     "${FIB_N}")"   | tee -a "$suite_csv"

# ---------- FIB REC ----------
echo "fib_rec      tenge        $(bench_cmd "tenge" .bin/fib_rec_cli "${FIB_N/90/35}")" | tee -a "$suite_csv"
echo "fib_rec      c            $(bench_cmd "c"     .bin/fib_rec_c   "${FIB_N/90/35}")" | tee -a "$suite_csv"
echo "fib_rec      rust         $(bench_cmd "rust"  .bin/fib_rec_rs  "${FIB_N/90/35}")" | tee -a "$suite_csv"
echo "fib_rec      go           $(bench_cmd "go"    .bin/fib_rec     "${FIB_N/90/35}")" | tee -a "$suite_csv"

# ---------- VaR MC ----------
echo "var_mc       tenge(sort)  $(bench_cmd "tenge(sort)" .bin/var_mc_tng_sort 1000000)" | tee -a "$suite_csv"
echo "var_mc       tenge(zig)   $(bench_cmd "tenge(zig)"  .bin/var_mc_tng_zig  1000000)" | tee -a "$suite_csv"
echo "var_mc       tenge(qsel)  $(bench_cmd "tenge(qsel)" .bin/var_mc_tng_qsel 1000000)" | tee -a "$suite_csv"
echo "var_mc       c            $(bench_cmd "c(-)"        .bin/var_monte_carlo_c 1000000)" | tee -a "$suite_csv"
echo "var_mc       rust         $(bench_cmd "rust(-)"     .bin/var_mc_rs       1000000)" | tee -a "$suite_csv"
echo "var_mc       go           $(bench_cmd "go(-)"       .bin/var_mc          1000000)" | tee -a "$suite_csv"
# accuracy CSV уже пишет C/Rust/Go бинарь — мы просто сохраняем их выводы в отдельный файл
# (при необходимости парсинга можно добавить сюда конкатенацию меток)

# ---------- N-BODY ----------
echo "nbody        tenge        $(bench_cmd "tenge" .bin/nbody_tng        "${NBODY_N}")" | tee -a "$suite_csv"
echo "nbody        c            $(bench_cmd "c"     .bin/nbody_c          "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}")" | tee -a "$suite_csv"
echo "nbody        rust         $(bench_cmd "rust"  .bin/nbody_rs         "${NBODY_N}")" | tee -a "$suite_csv"
echo "nbody        go           $(bench_cmd "go"    .bin/nbody            "${NBODY_N}")" | tee -a "$suite_csv"

echo "nbody_sym    tenge        $(bench_cmd "tenge(sym)" .bin/nbody_tng_sym    "${NBODY_N}")" | tee -a "$suite_csv"
echo "nbody_sym    c            $(bench_cmd "c(sym)"     .bin/nbody_sym_c      "${NBODY_N}" "${NBODY_STEPS}" "${NBODY_DT}")" | tee -a "$suite_csv"
echo "nbody_sym    rust         $(bench_cmd "rust(sym)"  .bin/nbody_rs_sym     "${NBODY_N}")" | tee -a "$suite_csv"
echo "nbody_sym    go           $(bench_cmd "go(sym)"    .bin/nbody_sym        "${NBODY_N}")" | tee -a "$suite_csv"

echo "[bench] All done."
echo "[bench] Summary: $suite_csv"
echo "[bench] Accuracy: $acc_csv"