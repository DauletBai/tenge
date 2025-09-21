package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

func main() {
	nObs := 10000
	if len(os.Args) > 1 {
		if n, err := strconv.Atoi(os.Args[1]); err == nil {
			nObs = n
		}
	}

	omega := 0.0001
	alpha := 0.1
	beta := 0.85

	t0 := time.Now()

	returns := make([]float64, nObs)
	variances := make([]float64, nObs)

	for i := 0; i < nObs; i++ {
		returns[i] = 0.01 * float64(i%100-50) / 50.0
		varPrev := omega / (1 - alpha - beta)
		if i > 0 {
			varPrev = variances[i-1]
		}
		variances[i] = omega + alpha*returns[i]*returns[i] + beta*varPrev
	}

	volSum := 0.0
	for i := 0; i < nObs; i++ {
		volSum += math.Sqrt(variances[i])
	}

	elapsed := time.Since(t0)
	fmt.Printf("TASK=garch,N=%d,TIME_NS=%d,VOL_SUM=%.6f\n", nObs, elapsed.Nanoseconds(), volSum)
}
