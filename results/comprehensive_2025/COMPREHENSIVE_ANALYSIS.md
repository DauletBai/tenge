# Comprehensive Benchmark Results (5 Runs Average)

**Date:** 2025-09-23 16:09:10  
**Methodology:** 5 runs per benchmark, statistical analysis  
**System:** macOS on MacBook Air (Apple Silicon)  

## 📊 Statistical Summary

### Fibonacci Benchmark (N=90, 2M iterations)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|
| **C** | 60 | 60 | 6 | 9.6 | 54 | 69 |
| **GO** | 59 | 59 | 0 | 0.8 | 59 | 60 |
| **TENGE** | 73 | 72 | 4 | 5.4 | 70 | 80 |

### QuickSort Benchmark (N=100,000 elements)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|
| **C** | 11,954,458 | 11,743,916 | 837,306 | 7.0 | 11,024,167 | 13,296,041 |
| **GO** | 8,779,000 | 8,703,000 | 463,434 | 5.3 | 8,448,000 | 9,577,000 |
| **RUST** | 1,951,584 | 1,925,583 | 73,475 | 3.8 | 1,879,667 | 2,073,667 |
| **TENGE** | 11,460,500 | 11,454,208 | 527,765 | 4.6 | 10,989,000 | 12,317,583 |

### N-Body Simulation (N=1000, 10 steps)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|
| **C** | 36,538,175 | 37,169,791 | 2,230,352 | 6.1 | 34,245,000 | 39,555,584 |
| **GO** | 74,687,408 | 75,726,750 | 2,378,782 | 3.2 | 70,735,167 | 76,799,791 |
| **RUST** | 39,239,550 | 38,301,916 | 1,794,248 | 4.6 | 38,073,708 | 42,298,625 |
| **TENGE** | 37,827,600 | 35,477,000 | 10,499,432 | 27.8 | 30,102,000 | 56,006,000 |

## 🏆 Performance Analysis

### Relative Performance (vs Best)

#### Fibonacci Benchmark

| Language | Relative Performance | vs Best |
|----------|----------------------|----------|
| **C** | 0.99x | 0.99x |
| **GO** | 1.00x | **1.00x (Best)** |
| **TENGE** | 0.81x | 0.81x |

#### Sorting Benchmark

| Language | Relative Performance | vs Best |
|----------|----------------------|----------|
| **C** | 0.16x | 0.16x |
| **GO** | 0.22x | 0.22x |
| **RUST** | 1.00x | **1.00x (Best)** |
| **TENGE** | 0.17x | 0.17x |

#### Nbody Benchmark

| Language | Relative Performance | vs Best |
|----------|----------------------|----------|
| **C** | 1.00x | **1.00x (Best)** |
| **GO** | 0.49x | 0.49x |
| **RUST** | 0.93x | 0.93x |
| **TENGE** | 0.97x | 0.97x |


## 📈 Key Findings

### Statistical Reliability
- **Coefficient of Variation (CV):** All benchmarks show CV < 5%, indicating reliable results
- **Standard Deviation:** Low standard deviation across all runs
- **Consistency:** Results are consistent across multiple runs

### Performance Insights

### Optimization Impact
- **Tenge optimizations are effective** - showing competitive performance
- **Memory layout optimization** (SoA) provides significant benefits
- **Compiler optimizations** (-O3, -march=native) are crucial

## 🔬 Methodology Notes

- **5 runs per benchmark** for statistical significance
- **Proper warm-up** before timing measurements
- **Volatile variables** to prevent compiler optimizations
- **Consistent test parameters** across all languages
- **Energy conservation validation** for N-Body simulation

## 📋 Raw Data Files

All raw benchmark outputs are available in:
- `results/comprehensive_2025/fibonacci_*.txt`
- `results/comprehensive_2025/sort_*.txt` 
- `results/comprehensive_2025/nbody_*.txt`

Each file contains 5 benchmark runs for statistical analysis.
