# Latest Benchmark Summary (20250925_143448)

- REPS: 5
- SIZE (sort): 100000
- FIB_N (iter): 90
- NBODY: N=4096, steps=10, dt=0.001

## Results CSV
- Suite: `benchmarks/results/suite_20250925_143448.csv`
- VaR Accuracy: `benchmarks/results/var_acc_20250925_143448.csv`

## Notes
- Tenge sort demos are built via AOT only if missing.
- Runtime arguments are passed **only at execution time**, never at build.
- All times are averages over REPS using `time.perf_counter_ns()`.
