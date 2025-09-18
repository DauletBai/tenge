# Benchmark Results (Averaged over 3 runs)

All numbers are **average runtime** in nanoseconds (ns).  
Hardware: Apple MacBook Air M2 (8 GB RAM, macOS).  
REPS = 5 for each run; results are averaged across 3 benchmark sessions.

---

## 1. Fibonacci Iterative (N=90)

| Language | Time (ns) |
|----------|-----------|
|**Tenge** |  **57.3** |
| C        |   374.7   |
| Rust     |   212.0   |
| Go       | 4,730.0   |

## 2. Fibonacci Recursive (N=35)

| Language | Time (ns)   |
|----------|-------------|
|**Tenge** |**44,496,000**|
| C        |  44,016,200 |
| Rust     |  45,687,800 |
| Go       |  50,803,800 |

## 3. Sort (N=100,000 integers, qsort baseline)

| Language         | Time (ns).   |
|------------------|--------------|
|**Tenge (qsort)** |**2,535,000** |
| C (qsort)        |  2,543,000   |
| Rust (std::sort) |  1,622,000   |
| Go (sort.Ints)   |  9,717,000   |

## 4. Monte Carlo VaR (N=1,000,000, α=0.99)

| Variant            | Time (ns)   |
|--------------------|-------------|
|**Tenge (qselect)** |**23,517,000**|
| Tenge (ziggurat)   | 116,245,000 |
| Tenge (sort)       | 214,940,000 |
| C                  | 214,874,000 |
| Rust               | 97,174,000  |
| Go                 | 203,843,000 |

## 5. N-body (N=4096, steps=10)

| Language | Time (ns)     |
|----------|---------------|
| **Tenge**| **364,200,000**|
| C        |   364,600,000 |
| Rust     |   480,300,000 |
| Go       | 1,028,000,000 |

## 6. Symmetric N-body (N=4096, steps=10)

| Language  | Time (ns)   |
|-----------|-------------|
| **Tenge** |**31,700,000**|
| C         |  32,200,000 |
| Rust      | 547,000,000 |
| Go        | 537,000,000 |

## Benchmark Results (averaged over 3 runs)

| Benchmark            |    Tenge (ns)|        C (ns)|     Rust (ns)|        Go (ns)|
|----------------------|-------------:|-------------:|-------------:|--------------:|
| Sort (qsort)         |   `2,532,800`|   `2,564,000`|   `1,624,000`|    `9,566,200`|
| Fib Iter (n=90)      |          `57`|         `491`|         `208`|        `5,383`|
| Fib Rec (n=35)       |  `45,188,400`|  `43,739,400`|  `45,393,183`|   `50,871,016`|
| VaR Monte Carlo sort | `215,344,000`| `215,454,200`|  `97,444,317`|  `205,289,566`|
| VaR Monte Carlo zig  | `116,540,800`|      –       |      –       |       –       |
| VaR Monte Carlo qsel |  `37,034,600`|      –       |      –       |       –       |
| N-Body               | `363,873,600`| `363,914,400`| `480,283,550`|`1,029,021,391`|
| N-Body (sym)         |  `47,702,133`|  `48,515,850`| `547,077,516`|  `537,666,241`|

---

## Observations
- **Tenge and C are neck-to-neck** on classical kernels (`sort`, `nbody`).
- **Tenge dominates Monte Carlo (qselect)**, achieving **9× faster** than Go, **4× faster** than Rust, and **~9× faster** than C.
- **Symmetric kernels** show Tenge/C outperforming Rust/Go by ~15–17×.
- Go consistently lags behind in compute-heavy workloads, while Rust shows strong but slightly slower performance than C/Tenge.
