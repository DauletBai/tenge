package main

import (
	"fmt"
	"math/rand"
	"os"
	"sort"
	"strconv"
	"time"
)

func nowNs() int64 {
	return time.Now().UnixNano()
}

func main() {
	N := 100000
	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil {
			N = v
		}
	}

	// Генерация случайного массива
	arr := make([]int, N)
	for i := 0; i < N; i++ {
		arr[i] = rand.Int()
	}

	t0 := nowNs()
	sort.Ints(arr)
	t1 := nowNs()

	fmt.Printf("TASK=sort,N=%d,TIME_NS=%d\n", N, (t1 - t0))
}