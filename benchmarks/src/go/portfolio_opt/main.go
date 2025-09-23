package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func main() {
	nAssets := 100
	if len(os.Args) > 1 {
		if n, err := strconv.Atoi(os.Args[1]); err == nil {
			nAssets = n
		}
	}

	t0 := time.Now()

	returns := make([]float64, nAssets)
	covMatrix := make([][]float64, nAssets)
	weights := make([]float64, nAssets)

	for i := 0; i < nAssets; i++ {
		covMatrix[i] = make([]float64, nAssets)
	}

	for i := 0; i < nAssets; i++ {
		returns[i] = 0.01 + 0.02*float64(i%10)/10.0
	}

	for i := 0; i < nAssets; i++ {
		for j := 0; j < nAssets; j++ {
			if i == j {
				covMatrix[i][j] = 0.04
			} else {
				covMatrix[i][j] = 0.01 * float64(i+j) / (2.0 * float64(nAssets))
			}
		}
	}

	for i := 0; i < nAssets; i++ {
		weights[i] = 1.0 / float64(nAssets)
	}

	portfolioVar := 0.0
	for i := 0; i < nAssets; i++ {
		for j := 0; j < nAssets; j++ {
			portfolioVar += weights[i] * weights[j] * covMatrix[i][j]
		}
	}

	elapsed := time.Since(t0)
	fmt.Printf("TASK=portfolio_opt,N=%d,TIME_NS=%d,PORTFOLIO_VAR=%.6f\n", nAssets, elapsed.Nanoseconds(), portfolioVar)
}

