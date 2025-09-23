// FILE: benchmarks/src/c/fib_rec.c
#include "runtime.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Explicit prototype to avoid implicit-decl warning on C99+ */
extern long long now_ns(void);

static uint64_t fib_rec(int n) {
    if (n <= 1) return (uint64_t)n;
    return fib_rec(n - 1) + fib_rec(n - 2);
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 35;

    long long t0 = now_ns();
    uint64_t r = fib_rec(n);
    long long t1 = now_ns();

    printf("TASK=fib_rec,N=%d,TIME_NS=%lld,RESULT=%llu\n",
           n, (t1 - t0), (unsigned long long)r);
    return 0;
}