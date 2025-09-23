// FILE: benchmarks/src/c/sort.c
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Explicit prototype to avoid implicit-decl warning on C99+ */
extern long long now_ns(void);

/* Deterministic xorshift64* RNG for reproducible data */
static inline uint64_t xs_step(uint64_t *s) {
    uint64_t x = *s;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *s = x;
    return x * 0x2545F4914F6CDD1DULL;
}

static int cmp_i64(const void *a, const void *b) {
    int64_t x = *(const int64_t*)a;
    int64_t y = *(const int64_t*)b;
    return (x > y) - (x < y);
}

int main(int argc, char **argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 100000;

    int64_t *arr = (int64_t*)malloc((size_t)N * sizeof(int64_t));
    if (!arr) {
        fprintf(stderr, "alloc failed\n");
        return 1;
    }

    uint64_t seed = 0x9E3779B97F4A7C15ULL;
    for (int i = 0; i < N; ++i) {
        arr[i] = (int64_t)xs_step(&seed);
    }

    long long t0 = now_ns();
    qsort(arr, (size_t)N, sizeof(int64_t), cmp_i64);
    long long t1 = now_ns();

    int ok = (N > 1 && arr[0] <= arr[N-1]) ? 1 : 0;

    printf("TASK=sort,N=%d,TIME_NS=%lld,OK=%d,LAST=%lld\n",
           N, (t1 - t0), ok, (long long)arr[N-1]);

    free(arr);
    return 0;
}