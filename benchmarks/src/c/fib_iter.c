// FILE: benchmarks/src/c/fib_iter_fixed.c
// Fixed version with same methodology as Tenge
#include "runtime.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Explicit prototype to avoid implicit-decl warning on C99+ */
extern long long now_ns(void);

/* Iterative Fibonacci with same methodology as Tenge */
int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 90;
    int reps = (argc > 2) ? atoi(argv[2]) : 2000000;
    
    if (reps <= 0) { reps = 2000000; }

    // Warm-up
    uint64_t a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        uint64_t t = a + b;
        a = b;
        b = t;
    }

    // Clean measurement - same as Tenge
    volatile uint64_t sink = 0;  // volatile to prevent optimization
    long long start = now_ns();
    for (int r = 0; r < reps; r++) {
        uint64_t a = 0, b = 1;
        for (int i = 0; i < n; ++i) {
            uint64_t t = a + b;
            a = b;
            b = t;
        }
        sink += b;  // accumulate result, don't just overwrite
    }
    long long end = now_ns();
    
    // Calculate average time per iteration (same as Tenge)
    long long total_time = end - start;
    long long avg_ns = total_time / (reps > 0 ? reps : 1);
    
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, avg_ns);
    return 0;
}

