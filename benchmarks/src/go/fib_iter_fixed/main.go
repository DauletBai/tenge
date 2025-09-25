// FILE: benchmarks/src/go/fib_iter_fixed/main.go
// Fixed version with same methodology as Tenge
package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

// fibIter computes Fibonacci numbers iteratively.
// Returns fib(n) for n >= 0 using 64-bit arithmetic.
func fibIter(n int) uint64 {
	if n < 2 {
		return uint64(n)
	}
	var a, b uint64 = 0, 1
	for i := 2; i <= n; i++ {
		a, b = b, a+b
	}
	return b
}

// parseN reads n from argv[1] if present, otherwise returns defaultN.
func parseN(defaultN int) int {
	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil && v >= 0 {
			return v
		}
	}
	return defaultN
}

// parseReps reads reps from argv[2] if present, otherwise returns defaultReps.
func parseReps(defaultReps int) int {
	if len(os.Args) > 2 {
		if v, err := strconv.Atoi(os.Args[2]); err == nil && v > 0 {
			return v
		}
	}
	return defaultReps
}

func main() {
	// Benchmark parameters
	n := parseN(90)
	reps := parseReps(2000000)

	// Warm-up
	_ = fibIter(10)

	// Clean measurement - same as Tenge
	var sink uint64 = 0
	start := time.Now()
	for r := 0; r < reps; r++ {
		sink += fibIter(n)
	}
	elapsed := time.Since(start)

	// Calculate average time per iteration (same as Tenge)
	avgNs := elapsed.Nanoseconds() / int64(reps)

	// Prevent the compiler from optimizing away the loop
	if sink == 0xDEADBEEF {
		fmt.Fprintln(os.Stderr, "unreachable")
	}

	// Output format matching Tenge
	fmt.Printf("TASK=fib_iter_fixed,N=%d,TIME_NS=%d\n", n, avgNs)
}
