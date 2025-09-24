#!/bin/bash

# Comprehensive Benchmark Suite - 5 runs for statistical significance
# Date: September 23, 2025

echo "🚀 Starting Comprehensive Benchmark Suite (5 runs)"
echo "=================================================="
echo "Date: $(date)"
echo "System: $(uname -a)"
echo ""

# Create results directory
mkdir -p results/comprehensive_2025

# Function to run benchmark and extract time
run_benchmark() {
    local name="$1"
    local cmd="$2"
    local output_file="$3"
    
    echo "Running: $name"
    echo "Command: $cmd"
    
    # Run 5 times and capture results
    for i in {1..5}; do
        echo "  Run $i/5..."
        result=$(eval "$cmd" 2>/dev/null)
        if [ $? -eq 0 ]; then
            echo "$result" >> "$output_file"
        else
            echo "ERROR: $name failed on run $i" >> "$output_file"
        fi
        sleep 1  # Brief pause between runs
    done
    echo ""
}

# Ensure all binaries are built
echo "🔨 Building all benchmarks..."
make build > /dev/null 2>&1
echo "Build complete."
echo ""

# Fibonacci benchmarks
echo "📊 FIBONACCI BENCHMARKS (N=90, 2M iterations)"
echo "============================================="

run_benchmark "C Fibonacci" \
    ".bin/fib_iter_c 90 2000000" \
    "results/comprehensive_2025/fibonacci_c.txt"

run_benchmark "Go Fibonacci" \
    ".bin/fib_iter_go 90 2000000" \
    "results/comprehensive_2025/fibonacci_go.txt"

run_benchmark "Rust Fibonacci" \
    ".bin/fib_iter_rs 90 2000000" \
    "results/comprehensive_2025/fibonacci_rust.txt"

run_benchmark "Tenge Fibonacci" \
    ".bin/fib_cli 90 2000000" \
    "results/comprehensive_2025/fibonacci_tenge.txt"

# Sorting benchmarks
echo "📊 SORTING BENCHMARKS (N=100,000 elements)"
echo "=========================================="

run_benchmark "C QuickSort" \
    ".bin/sort_c 100000" \
    "results/comprehensive_2025/sort_c.txt"

run_benchmark "Go QuickSort" \
    ".bin/sort_go 100000" \
    "results/comprehensive_2025/sort_go.txt"

run_benchmark "Rust QuickSort" \
    ".bin/sort_rs 100000" \
    "results/comprehensive_2025/sort_rust.txt"

run_benchmark "Tenge QuickSort" \
    ".bin/sort_tenge 100000" \
    "results/comprehensive_2025/sort_tenge.txt"

# N-Body benchmarks
echo "📊 N-BODY SIMULATION BENCHMARKS (N=1000, 10 steps)"
echo "=================================================="

run_benchmark "C N-Body" \
    ".bin/nbody_c 1000 10" \
    "results/comprehensive_2025/nbody_c.txt"

run_benchmark "Go N-Body" \
    ".bin/nbody_go 1000 10" \
    "results/comprehensive_2025/nbody_go.txt"

run_benchmark "Rust N-Body" \
    ".bin/nbody_rs 1000 10" \
    "results/comprehensive_2025/nbody_rust.txt"

run_benchmark "Tenge N-Body (Optimized)" \
    ".bin/nbody_tng 1000 10" \
    "results/comprehensive_2025/nbody_tenge.txt"

echo "✅ All benchmarks completed!"
echo "Results saved in: results/comprehensive_2025/"
echo ""
echo "📈 Generating analysis report..."

