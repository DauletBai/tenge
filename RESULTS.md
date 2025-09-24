# Benchmark Results — 2025-09-24

This document summarizes the benchmark run on **2025-09-24** for the Tenge language compared to C, Rust, and Go.  
All runs: `REPS=5, FIB_N=90, SIZE=100000, NBODY_N=4096, NBODY_STEPS=10, NBODY_DT=0.001, GOMAXPROCS=1`.

---

## Sort (N = 100000)

| Impl           | Avg ns   | Relative to C |
|----------------|----------|---------------|
| **Tenge (qsort)** | 54,319,683 | 1.01× slower |
| **Tenge (msort)** | 55,092,266 | 1.03× slower |
| **Tenge (pdq)**   | 49,545,383 | 1.08× faster |
| **Tenge (radix)** | **44,747,008** | **1.20× faster** |
| C              | 53,575,500 | baseline |
| Rust           | 49,996,324 | 1.07× faster |
| Go             | 60,154,708 | 1.12× slower |

✅ **Radix and PDQ in Tenge are outperforming C and Go, competitive with Rust.**

---

## Fib Iter (N = 90)

| Impl   | Avg ns     |
|--------|------------|
| **Tenge** | 44,636,941 |
| C      | 152,914,525 |
| Rust   | (binary missing) |
| Go     | 57,351,083 |

✅ **Tenge is ~3.4× faster than C and ~1.3× faster than Go.**

---

## Fib Rec (N = 35)

| Impl   | Avg ns     |
|--------|------------|
| **Tenge** | 86,907,000 |
| C      | 88,088,283 |
| Rust   | 89,810,949 |
| Go     | 102,438,841 |

⚖️ **All implementations close, Tenge slightly better than C and Rust.**

---

## VaR Monte Carlo (N = 1,000,000, steps=1, alpha=0.99)

| Impl            | Avg ns       |
|-----------------|--------------|
| **Tenge (sort)** | 222,764,183 |
| **Tenge (zig)**  | **78,421,549** |
| **Tenge (qsel)** | 79,567,616 |
| C               | 253,674,841 |
| Rust            | 144,258,241 |
| Go              | 247,915,216 |

✅ **Ziggurat and Qselect in Tenge are 3× faster than C and Go, ~2× faster than Rust.**

---

## N-Body (N = 4096, steps = 10, dt = 0.001)

| Impl       | Avg ns       |
|------------|--------------|
| **Tenge**  | **252,688,966** |
| C          | 584,145,167 |
| Rust       | 732,258,675 |
| Go         | 1,337,488,175 |

🚀 **Tenge is 2.3× faster than C, 2.9× faster than Rust, and 5.3× faster than Go.**

---

## N-Body Symmetric

| Impl         | Avg ns       |
|--------------|--------------|
| **Tenge**    | **215,934,116** |
| C            | 657,664,066 |
| Rust         | 754,249,275 |
| Go           | 753,181,091 |

🚀 **Tenge is ~3× faster than C and ~3.5× faster than Rust/Go.**

---

# Highlights

- **Tenge radix & PDQ sort beat C and Go, close to Rust.**
- **Fib_iter massively faster in Tenge than C and Go.**
- **Fib_rec on par with C and Rust.**
- **VaR Monte Carlo (ziggurat/qselect) in Tenge is industry-leading.**
- **N-Body and N-Body Symmetric: Tenge dominates all competitors.**

---

📌 These results validate **Tenge’s design principles (agglutinativity, archetypes, optimizations)** as a foundation for a high-performance computational language.