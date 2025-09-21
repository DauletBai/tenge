package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func main() {
	matrixSize := 200
	if len(os.Args) > 1 {
		if n, err := strconv.Atoi(os.Args[1]); err == nil {
			matrixSize = n
		}
	}

	t0 := time.Now()

	matrixA := make([][]float64, matrixSize)
	matrixB := make([][]float64, matrixSize)
	matrixC := make([][]float64, matrixSize)

	for i := 0; i < matrixSize; i++ {
		matrixA[i] = make([]float64, matrixSize)
		matrixB[i] = make([]float64, matrixSize)
		matrixC[i] = make([]float64, matrixSize)
	}

	for i := 0; i < matrixSize; i++ {
		for j := 0; j < matrixSize; j++ {
			matrixA[i][j] = float64(i+j) * 0.01
			matrixB[i][j] = float64(i-j) * 0.01
		}
	}

	for i := 0; i < matrixSize; i++ {
		for j := 0; j < matrixSize; j++ {
			sum := 0.0
			for k := 0; k < matrixSize; k++ {
				sum += matrixA[i][k] * matrixB[k][j]
			}
			matrixC[i][j] = sum
		}
	}

	trace := 0.0
	for i := 0; i < matrixSize; i++ {
		trace += matrixC[i][i]
	}

	elapsed := time.Since(t0)
	fmt.Printf("TASK=matrix_ops,N=%d,TIME_NS=%d,TRACE=%.6f\n", matrixSize, elapsed.Nanoseconds(), trace)
}
