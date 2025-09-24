package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func fib(n int) uint64 {
	if n <= 1 {
		return uint64(n)
	}
	return fib(n-1) + fib(n-2)
}

func main() {
	n := 35
	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil {
			n = v
		}
	}
	batch := 0
	if v := os.Getenv("BATCH_ITER"); v != "" {
		if b, err := strconv.Atoi(v); err == nil {
			batch = b
		}
	}
	run := func() uint64 { return fib(n) }

	start := time.Now()
	iters := 1
	if batch > 0 {
		iters = batch
	}
	var acc uint64
	for i := 0; i < iters; i++ {
		acc ^= run()
	}
	elapsed := time.Since(start).Nanoseconds()
	fmt.Printf("TASK=fib_rec,N=%d,TIME_NS=%d,ACC=%d\n", n, elapsed, acc)
}