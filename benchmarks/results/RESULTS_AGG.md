# Aggregated Benchmarks (averaged across provided CSVs)

_Language columns ordered as: Tenge → C → Rust → Go._

## fib_iter

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 90 | — | 57 ns | 31771733 ns ± 85855 | 32466933 ns ± 49710 | 33578600 ns ± 540655 |

## fib_rec

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 35 | — | 44089400 ns ± 125259 | 75649200 ns ± 82383 | 78129800 ns ± 123061 | 83891267 ns ± 555545 |

## nbody

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 4096 | — | 496808067 ns ± 10063397 | 462831000 ns ± 423878 | 586610467 ns ± 5762847 | 1202004133 ns ± 1260969 |

## nbody_sym

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 4096 | — | 111061267 ns ± 820354 | 586121200 ns ± 8657839 | 663222533 ns ± 5539699 | 644745933 ns ± 1089361 |

## sort

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 100000 | — | 71409667 ns ± 31967653 | 47210333 ns ± 4366645 | 36483800 ns ± 381669 | 43263533 ns ± 483939 |

## var_mc

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 1000000 | — | — | 247129267 ns ± 395210 | 128570467 ns ± 388531 | 236996067 ns ± 499064 |

## var_mc_qsel

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 1000000 | qsel | 91736533 ns ± 198916 | — | — | — |

## var_mc_sort

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 1000000 | sort | 284635667 ns ± 1121550 | — | — | — |

## var_mc_zig

| N | Variant | Tenge | C | Rust | Go |
|---|---|---|---|---|---|
| 1000000 | zig | 174017267 ns ± 1379152 | — | — | — |

