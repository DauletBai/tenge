// Fixed version with same methodology as Tenge
package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func main() {
	n := 90
	reps := 2000000

	if len(os.Args) > 1 {
		if val, err := strconv.Atoi(os.Args[1]); err == nil {
			n = val
		}
	}
	if len(os.Args) > 2 {
		if val, err := strconv.Atoi(os.Args[2]); err == nil {
			reps = val
		}
	}

	if reps <= 0 {
		reps = 2000000
	}

	// Warm-up
	a, b := uint64(0), uint64(1)
	for i := 0; i < n; i++ {
		t := a + b
		a = b
		b = t
	}

	// Clean measurement - same as Tenge
	var sink uint64 = 0 // accumulate result to prevent optimization
	start := time.Now()
	for r := 0; r < reps; r++ {
		a, b := uint64(0), uint64(1)
		for i := 0; i < n; i++ {
			t := a + b
			a = b
			b = t
		}
		sink += b // accumulate result, don't just discard
	}
	end := time.Now()

	// Calculate average time per iteration (same as Tenge)
	totalTime := end.Sub(start)
	avgNs := totalTime.Nanoseconds() / int64(reps)

	fmt.Printf("TASK=fib_iter,N=%d,TIME_NS=%d\n", n, avgNs)
}
