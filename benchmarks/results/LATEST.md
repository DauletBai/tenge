# Tenge Bench – latest (20250924_103857)

REPS=5, SIZE_SORT=100000, FIB_N_ITER=90, FIB_N_REC=35, VAR_N=1000000, VAR_STEPS=1, VAR_ALPHA=0.99, NBODY_N=4096, NBODY_STEPS=10, NBODY_DT=0.001

Results CSV: `benchmarks/results/suite_20250924_103857.csv`

Accuracy CSV (VaR): `benchmarks/results/var_acc_20250924_103857.csv`

| Task | Lang | Variant | N | Extra | Avg ns |
|---|---|---|---:|---|---:|
| sort | c | - | 100000 |  | 12020516 |
| sort | rust | - | 100000 |  | 2499100 |
| sort | go | - | 100000 |  | 8646400 |
| fib_iter | tenge | - | 90 |  | 0 |
| fib_iter | c | - | 90 |  | 0 |
| fib_iter | go | - | 90 |  | 0 |
| fib_rec | tenge | - | 35 |  | 44101308 |
| fib_rec | c | - | 35 |  | 42924108 |
| fib_rec | rust | - | 35 |  | 43152075 |
| fib_rec | go | - | 35 |  | 49961466 |
| var_mc | tenge | sort | 1000000 | alpha=0.99 | 183408958 |
| var_mc | tenge | zig | 1000000 | alpha=0.99 | 32286141 |
| var_mc | tenge | qsel | 1000000 | alpha=0.99 | 34089516 |
| var_mc | c | - | 1000000 | alpha=0.99 | 207430600 |
| var_mc | rust | - | 1000000 | alpha=0.99 | 94548875 |
| var_mc | go | - | 1000000 | alpha=0.99 | 192835800 |
| nbody | tenge | - | 4096 | steps=10 | 210929825 |
| nbody | c | - | 4096 | steps=10 | 555846150 |
| nbody | rust | - | 4096 | steps=10 | 595595483 |
| nbody | go | - | 4096 | steps=10 | 1147953658 |
| nbody_sym | tenge | - | 4096 | steps=10 | 188078891 |
| nbody_sym | c | - | 4096 | steps=10 | 634399475 |
| nbody_sym | rust | - | 4096 | steps=10 | 622589866 |
| nbody_sym | go | - | 4096 | steps=10 | 648511641 |
