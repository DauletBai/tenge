// benchmarks/src/go/var_mc_acc/main.go
// Accuracy benchmark for Value-at-Risk (VaR) estimation.

package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

type rng struct{ s uint64 }

func (r *rng) Seed(seed uint64) {
	if seed == 0 {
		seed = 0x123456789ABCDEF0
	}
	r.s = seed
}
func (r *rng) U64() uint64 {
	x := r.s
	x ^= x >> 12
	x ^= x << 25
	x ^= x >> 27
	r.s = x
	return x * 0x2545F4914F6CDD1D
}
func (r *rng) Uniform() float64 {
	return float64(r.U64()>>11) * (1.0 / 9007199254740992.0)
}

func invnorm(p float64) float64 {
	if p <= 0.0 {
		p = 1e-16
	}
	if p >= 1.0 {
		p = 1.0 - 1e-16
	}
	return -math.Sqrt(2.0) * math.Sqrt(math.Log(2.0*p))
}

func main() {
	N := 1000000
	mu := 0.0
	sigma := 1.0
	alpha := 0.99

	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil {
			N = v
		}
	}
	if len(os.Args) > 2 {
		if v, err := strconv.ParseFloat(os.Args[2], 64); err == nil {
			mu = v
		}
	}
	if len(os.Args) > 3 {
		if v, err := strconv.ParseFloat(os.Args[3], 64); err == nil {
			sigma = v
		}
	}
	if len(os.Args) > 4 {
		if v, err := strconv.ParseFloat(os.Args[4], 64); err == nil {
			alpha = v
		}
	}

	start := time.Now()

	var r rng
	r.Seed(123456789)
	acc := 0.0

	for i := 0; i < N; i++ {
		u1 := r.Uniform()
		u2 := r.Uniform()
		r_val := math.Sqrt(-2.0 * math.Log(u1+1e-18))
		z := r_val * math.Cos(2.0*math.Pi*u2)
		pnl := mu + sigma*z
		acc += pnl
	}

	q := mu + sigma*invnorm(alpha)
	elapsed := time.Since(start)

	fmt.Printf("TASK=var_mc_acc,N=%d,TIME_NS=%d,ALPHA=%.6f,VAR=%.12f,ACC=%.6f\n",
		N, elapsed.Nanoseconds(), alpha, q, acc/float64(N))
}

