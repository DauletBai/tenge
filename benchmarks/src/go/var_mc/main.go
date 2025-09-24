package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

func main() {
	N := 1000000
	steps := 1
	alpha := 0.99

	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil {
			N = v
		}
	}
	if len(os.Args) > 2 {
		if v, err := strconv.Atoi(os.Args[2]); err == nil {
			steps = v
		}
	}
	if len(os.Args) > 3 {
		if v, err := strconv.ParseFloat(os.Args[3], 64); err == nil {
			alpha = v
		}
	}

	batch := 0
	if v := os.Getenv("BATCH_ITER"); v != "" {
		if b, err := strconv.Atoi(v); err == nil {
			batch = b
		}
	}

	genLoss := func() float64 {
		// простая модель: сумма lognormal шагов (быстро и репликабельно)
		mu, sigma := 0.0, 0.02
		x := 0.0
		for i := 0; i < steps; i++ {
			u1 := rand.Float64()
			u2 := rand.Float64()
			z := math.Sqrt(-2.0*math.Log(u1)) * math.Cos(2*math.Pi*u2)
			r := mu + sigma*z
			x += r
		}
		return -x // убыток
	}

	start := time.Now()
	iters := 1
	if batch > 0 {
		iters = batch
	}
	acc := 0.0
	for k := 0; k < iters; k++ {
		// вместо сортировки — квикселект "наивный" через порог
		// для простоты сейчас считаем реальную сортировку (O(N log N))
		// чтобы не зависеть от внешних утилит: возьмем топ по квантилю
		data := make([]float64, N)
		for i := 0; i < N; i++ {
			data[i] = genLoss()
		}
		// квантиль через частичную сортировку: простой nth_element-подобный подход
		q := int(math.Ceil(alpha*float64(N))) - 1
		// быстрая оценка: используем встроенную сортировку для стабильности
		// (для бенча точности/скорости раннер сравнит с C/Rust)
		sortFloats(data)
		var99 := data[q]
		acc += var99
	}
	elapsed := time.Since(start).Nanoseconds()
	fmt.Printf("TASK=var_mc,N=%d,TIME_NS=%d,ACC=%.6f\n", N, elapsed, acc)
}

// простая сортировка (introsort у Go runtime)
func sortFloats(a []float64) {
	// встроенная быстрая реализация
	// заменим на sort.Float64s без импорта лишнего кода
	type f64Slice []float64
	var s f64Slice = a
	// вставим локальную быструю сортировку (quick + heap) — для краткости используем стандартный подход:
	// но чтобы не тянуть "sort", делаем простую stdlib-like быструю сортировку:
	quickSort(s, 0, len(s)-1)
}

func quickSort(a []float64, l, r int) {
	for l < r {
		i, j := l, r
		p := a[(l+r)>>1]
		for i <= j {
			for a[i] < p { i++ }
			for a[j] > p { j-- }
			if i <= j {
				a[i], a[j] = a[j], a[i]
				i++; j--
			}
		}
		if (j - l) < (r - i) {
			if l < j { quickSort(a, l, j) }
			l = i
		} else {
			if i < r { quickSort(a, i, r) }
			r = j
		}
	}
}