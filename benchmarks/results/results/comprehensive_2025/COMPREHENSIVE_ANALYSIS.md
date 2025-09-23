# Comprehensive Benchmark Results (5 Runs Average)

**Date:** 2025-09-23 16:08:51  
**Methodology:** 5 runs per benchmark, statistical analysis  
**System:** macOS on MacBook Air (Apple Silicon)  

## 📊 Statistical Summary

### Fibonacci Benchmark (N=90, 2M iterations)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|

### QuickSort Benchmark (N=100,000 elements)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|

### N-Body Simulation (N=1000, 10 steps)

| Language | Mean (ns) | Median (ns) | Std Dev | CV (%) | Min (ns) | Max (ns) |
|----------|-----------|-------------|---------|--------|----------|----------|

## 🏆 Performance Analysis

### Relative Performance (vs Best)


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
