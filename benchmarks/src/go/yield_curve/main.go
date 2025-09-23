package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

func main() {
	nPoints := 1000
	if len(os.Args) > 1 {
		if n, err := strconv.Atoi(os.Args[1]); err == nil {
			nPoints = n
		}
	}

	beta0 := 0.05
	beta1 := -0.02
	beta2 := 0.01
	tau := 2.0

	t0 := time.Now()
	sum := 0.0

	for i := 0; i < nPoints; i++ {
		t := float64(i+1) * 0.1
		yieldVal := beta0 + beta1*(1-math.Exp(-t/tau))/(t/tau) +
			beta2*((1-math.Exp(-t/tau))/(t/tau)-math.Exp(-t/tau))
		sum += yieldVal
	}

	elapsed := time.Since(t0)
	fmt.Printf("TASK=yield_curve,N=%d,TIME_NS=%d,SUM=%.6f\n", nPoints, elapsed.Nanoseconds(), sum)
}

