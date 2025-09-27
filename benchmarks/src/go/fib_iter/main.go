package main

import (
	"fmt"
	"os"
	"strconv"
)

func fibIter(n uint64) uint64 {
	var a, b uint64 = 0, 1
	for i := uint64(0); i < n; i++ {
		a, b = b, a+b
	}
	return a
}

var sinkGlobal uint64

func main() {
	var n uint64 = 90
	if len(os.Args) > 1 {
		if v, err := strconv.ParseUint(os.Args[1], 10, 64); err == nil {
			n = v
		}
	}
	var inner uint64 = 0
	if v := os.Getenv("INNER_REPS"); v != "" {
		if ir, err := strconv.ParseUint(v, 10, 64); err == nil {
			inner = ir
		}
	}
	if inner == 0 {
		sinkGlobal ^= fibIter(n)
	} else {
		var s uint64 = 0
		for i := uint64(0); i < inner; i++ {
			s ^= fibIter(n)
		}
		sinkGlobal ^= s
	}
	// По умолчанию — тишина.
	if os.Getenv("PRINT_SINK") == "1" {
		fmt.Println(sinkGlobal)
	}
}