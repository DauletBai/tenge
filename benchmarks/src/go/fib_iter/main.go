// FILE: benchmarks/src/go/fib_iter/main.go
// Purpose: Iterative Fibonacci microbenchmark with per-operation timing.
// Notes:
// - Prints TIME_NS as average per single fib(n) evaluation to avoid 0 ns on fast runs.
// - Output format matches the unified runner: TASK=fib_iter_go,N=<n>,TIME_NS=<ns>

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

func main() {
	// Benchmark parameter: Fibonacci index.
	n := parseN(90)

	// Simple warm-up to stabilize CPU frequency and caches.
	_ = fibIter(10)

	// Auto-batching loop to avoid sub-microsecond timestamps.
	// Grows the number of iterations until total time >= 2ms or cap is reached.
	iters := 1
	const maxIters = 1_000_000
	var perOpNS int64

	for {
		start := time.Now()
		var sink uint64
		for i := 0; i < iters; i++ {
			sink += fibIter(n)
		}
		elapsed := time.Since(start)

		// Prevent the compiler from optimizing away the loop.
		if sink == 0xDEADBEEF {
			fmt.Fprintln(os.Stderr, "unreachable")
		}

		if elapsed >= 2*time.Millisecond || iters >= maxIters {
			perOpNS = elapsed.Nanoseconds() / int64(iters)
			break
		}
		// Exponentially increase the batch size until we cross the timing threshold.
		iters *= 4
		if iters > maxIters {
			iters = maxIters
		}
	}

	// Unified runner line (consumed by the bash runner).
	fmt.Printf("TASK=fib_iter_go,N=%d,TIME_NS=%d\n", n, perOpNS)
}