package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

func main() {
	nPoints := 1024
	if len(os.Args) > 1 {
		if n, err := strconv.Atoi(os.Args[1]); err == nil {
			nPoints = n
		}
	}

	t0 := time.Now()

	signal := make([]float64, nPoints)
	fftReal := make([]float64, nPoints)
	fftImag := make([]float64, nPoints)

	for i := 0; i < nPoints; i++ {
		t := float64(i) * 2.0 * math.Pi / float64(nPoints)
		signal[i] = math.Sin(t) + 0.5*math.Sin(3*t) + 0.25*math.Sin(5*t)
	}

	for k := 0; k < nPoints; k++ {
		realSum := 0.0
		imagSum := 0.0
		for n := 0; n < nPoints; n++ {
			angle := -2.0 * math.Pi * float64(k) * float64(n) / float64(nPoints)
			realSum += signal[n] * math.Cos(angle)
			imagSum += signal[n] * math.Sin(angle)
		}
		fftReal[k] = realSum
		fftImag[k] = imagSum
	}

	powerSum := 0.0
	for i := 0; i < nPoints; i++ {
		power := fftReal[i]*fftReal[i] + fftImag[i]*fftImag[i]
		powerSum += power
	}

	elapsed := time.Since(t0)
	fmt.Printf("TASK=fft,N=%d,TIME_NS=%d,POWER_SUM=%.6f\n", nPoints, elapsed.Nanoseconds(), powerSum)
}
